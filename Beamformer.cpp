#include "Beamformer.h"
#include <cstring>
#include <random>
#include <fstream>
#include <sys/time.h>

#define __COLLECT_DATA__
//#define __TIME_STAMP__

#define PREDFINED_RN16_ 0xAAAA

#define SIC_PORT_NUM_ ant_nums[ant_amount-1]


double normal_random(double mean, double std_dev){
  static std::random_device r;
  static std::default_random_engine generator(r());
  std::normal_distribution<double> distribution(mean, std_dev);

  return distribution(generator);
}


int Beamformer::run_beamformer(void){

  char buffer[IO_BUF_SIZE] = {};
  struct average_corr_data data;

  /******************** SIC measure stage *************/

  SIC_port_measure();
  
  if(ipc.data_recv(buffer) == -1){
    std::cerr <<"Breaker is activated"<<std::endl;
    return 0;
  } 
  memcpy(&data, buffer, sizeof(data));

  sic_ctrl = new SIC_controller(std::complex<float>(data.cw_i, data.cw_q));
  SIC_port_measure_over();


  /*****************************************************/

  //initial phase here
  
  if(ipc.send_ack() == -1)
    return 0;

  //loop until it is over
  while(1){
    /******************* SIC stage *******************/
    if(ipc.data_recv(buffer) == -1){
      std::cerr <<"Breaker is activated"<<std::endl;
      break;   
    } 
    memcpy(&data, buffer, sizeof(data));

    dataLogging(data);
    SIC_handler(data);    

    //send ack so that Gen2 program can recognize that the beamforming has been done
    if(ipc.send_ack() == -1){
      break;
    }
    /*************************************************/



    /******************* Signal stage *****************/
    if(ipc.data_recv(buffer) == -1){
      std::cerr <<"Breaker is activated"<<std::endl;
      break;   
    } 
    memcpy(&data, buffer, sizeof(data));

    dataLogging(data);
    Signal_handler(data);

    //send ack so that Gen2 program can recognize that the beamforming has been done
    if(ipc.send_ack() == -1){
      break;
    }
    /******************************************************/
  }//end of while(1)

  //print wait
  weights_printing(cur_weights);

  return 0;
}



Beamformer::Beamformer(Phase_Attenuator_controller * controller_p, int ant_amount_p, int * ant_num_p){
  this->phase_ctrl = controller_p;
  this->ant_amount = ant_amount_p;
  this->ant_nums = new int[this->ant_amount];

  log.open("log.csv");
  for(int i = 0; i<ant_amount; i++){
    log<<"phase "<<ant_nums[i]<<", ";
  }
  log<<"avg corr,i,q,RN16"<<std::endl;

  memcpy(ant_nums, ant_num_p, sizeof(int)*(this->ant_amount));
}



Beamformer::~Beamformer(){
  delete this->ant_nums;
  log.close();
}

int Beamformer::init_beamformer(void){
  BWtrainer = new Adaptive_beamtrainer(ant_amount-1);

  std::vector<int> weightVector = BWtrainer->startTraining();
  vector2cur_weights(weightVector);
  for(int i = 0; i<ant_amount; i++){
    phase_ctrl->ant_on(ant_nums[i]);
  }
  std::cout<<"Init"<<std::endl;
  return weights_apply(cur_weights);
}


int Beamformer::weights_addition(int * dest_weights, int * weights0, int * weights1){
  for(int i = 0; i<ant_amount-1; i++){
    dest_weights[ant_nums[i]] = weights0[ant_nums[i]] + weights1[ant_nums[i]];
    while(dest_weights[ant_nums[i]] < 0)
      dest_weights[ant_nums[i]]+= 360;
    dest_weights[ant_nums[i]] %= 360;
  }

  return 0;
}


int Beamformer::weights_addition(int * dest_weights, int * weights){
  for(int i = 0; i<ant_amount-1; i++){
    dest_weights[ant_nums[i]] += weights[ant_nums[i]];
    while(dest_weights[ant_nums[i]] < 0)
      dest_weights[ant_nums[i]]+= 360;
    dest_weights[ant_nums[i]] %= 360;
  }
  return 0;
}


int Beamformer::weights_apply(int * weights){
  for(int i = 0; i<ant_amount-1; i++){
    phase_ctrl->phase_control(ant_nums[i], weights[ant_nums[i]]);
  }
  return phase_ctrl->data_apply();
}


int Beamformer::weights_apply(void){
  int * weights = cur_weights;
  for(int i = 0; i<ant_amount; i++){
    phase_ctrl->phase_control(ant_nums[i], weights[ant_nums[i]]);
  }
  return phase_ctrl->data_apply();
}


int Beamformer::vector2cur_weights(std::vector<int> weightVector){
  for(int i = 0; i<ant_amount;i++){
    cur_weights[ant_nums[i]] = weightVector[i];
  }
  return 0;
}



void Beamformer::weights_printing(int * weights){
  for(int i = 0; i<ant_amount; i++){
    std::cout<<"ANT num : "<<ant_nums[i]<<std::endl;
    std::cout<<"Phase : "<<weights[ant_nums[i]]<<std::endl<<std::endl;
  }
}


int Beamformer::start_beamformer(void){
  if(init_beamformer())
    std::cerr << "Beamformer init failed"<<std::endl;
  std::cout << "Beamformer init finished"<<std::endl;
  if(ipc.wait_sync())
    return -1;

  return run_beamformer();
}


int Beamformer::SIC_port_measure(void){
  //We must measure SIC port before we start.
  for(int i = 0; i<16; i++){
    phase_ctrl->ant_off(i);
  }
  cur_weights[SIC_PORT_NUM_] = 0;
  phase_ctrl->ant_on(SIC_PORT_NUM_);
  phase_ctrl->phase_control(SIC_PORT_NUM_, SIC_REF_POWER, 0);
  phase_ctrl->data_apply();
  std::cout << "SIC Phase Set"<<std::endl;

  return 0;
}



int Beamformer::SIC_port_measure_over(void){
  //We must measure SIC port before we start.
  for(int i = 0; i<ant_amount-1; i++){
    phase_ctrl->ant_on(ant_nums[i]);
  }
  
  weights_apply(cur_weights);

  std::cout << "SIC over"<<std::endl;

  return 0;
}

int Beamformer::SIC_handler(struct average_corr_data & data){
  sic_ctrl->setCurrentAmp(std::complex<float>(data.cw_i, data.cw_q));
  cur_weights[SIC_PORT_NUM_] = sic_ctrl->getPhase();   //get SIC phase
  phase_ctrl->phase_control(SIC_PORT_NUM_, sic_ctrl->getPower(), cur_weights[SIC_PORT_NUM_]); //change phase and power
  phase_ctrl->data_apply();

  return 0;
}


int Beamformer::Signal_handler(struct average_corr_data & data){
  uint16_t tag_id = 0;
  std::vector<int> weightVector;


  for(int i = 0; i<16; i++){
    tag_id = tag_id << 1;
    tag_id += data.RN16[i];
  }

  /*************************Add algorithm here***************************/
  if(data.successFlag == _SUCCESS){

    for(int i = 0; i<16; i++){
      tag_id = tag_id << 1;
      tag_id += data.RN16[i];
    }

    printf("Got RN16 : %x\n",tag_id);
    printf("avg corr : %f\n",data.avg_corr);
    printf("avg iq : %f, %f\n",data.avg_i, data.avg_q);
    printf("avg amp : %f, %f\n\n",data.cw_i, data.cw_q);


    weightVector = BWtrainer->getRespond(data);
    vector2cur_weights(weightVector);
    if(weights_apply(cur_weights)){
      std::cerr<<"weight apply failed"<<std::endl;
      return 1;
    }
  }else{
    printf("Couldn't get RN16\n\n");

    weightVector = BWtrainer->cannotGetRespond();
    vector2cur_weights(weightVector);
    if(weights_apply(cur_weights)){
      std::cerr<<"weight apply failed"<<std::endl;
      return 1;
    }
  }
  /*****************************************************************/
  
  return 0;
}


int Beamformer::dataLogging(struct average_corr_data & data){
  uint16_t tag_id = 0;

  for(int i = 0; i<16; i++){
    tag_id = tag_id << 1;
    tag_id += data.RN16[i];
  }

  if(data.successFlag == _SUCCESS){
    for(int i = 0; i<ant_amount;i++){
      log<<cur_weights[ant_nums[i]]<<", ";
    }
    log<<data.avg_corr<<", "<<data.avg_i<<", "<<data.avg_q<<", "<<data.cw_i<<", "<<data.cw_q<<", "<<tag_id<<", "<<data.round<<std::endl;
  }else if(data.successFlag == _PREAMBLE_FAIL){
    for(int i = 0; i<ant_amount;i++){
      log<<cur_weights[ant_nums[i]]<<", ";
    }
    log<<0.0<<", "<<0.0<<", "<<0.0<<","<<data.cw_i<<", "<<data.cw_q<<", "<<"-"<<","<<data.round<<std::endl;
  }

  return 0;
}
