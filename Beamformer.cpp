#include "Beamformer.h"
#include <cstring>
#include <random>
#include <fstream>
#include <sys/time.h>
#include "Adaptive_Beamtrainer.h"

#define __COLLECT_DATA__
//#define __TIME_STAMP__

#define PREDFINED_RN16_ 0xAAAA



double normal_random(double mean, double std_dev){
  static std::random_device r;
  static std::default_random_engine generator(r());
  std::normal_distribution<double> distribution(mean, std_dev);

  return distribution(generator);
}



Beamformer::Beamformer(Phase_Attenuator_controller * controller_p, int ant_amount_p, int * ant_num_p){
  this->phase_ctrl = controller_p;
  this->ant_amount = ant_amount_p;
  this->ant_nums = new int[this->ant_amount];

  log.open("log.csv");

  memcpy(ant_nums, ant_num_p, sizeof(int)*(this->ant_amount));
}



Beamformer::~Beamformer(){
  delete this->ant_nums;
  log.close();
}

int Beamformer::init_beamformer(void){
  for(int i = 0; i < ant_amount; i++){
    int phase = normal_random(0, 180);
    while(phase < 0)
      phase += 360;
    phase %= 360;

    cur_weights[ant_nums[i]] = phase;
  }

  return weights_apply(cur_weights);
}


int Beamformer::weights_addition(int * dest_weights, int * weights0, int * weights1){
  for(int i = 0; i<ant_amount; i++){
    dest_weights[ant_nums[i]] = weights0[ant_nums[i]] + weights1[ant_nums[i]];
    while(dest_weights[ant_nums[i]] < 0)
      dest_weights[ant_nums[i]]+= 360;
    dest_weights[ant_nums[i]] %= 360;
  }

  return 0;
}



int Beamformer::weights_addition(int * dest_weights, int * weights){
  for(int i = 0; i<ant_amount; i++){
    dest_weights[ant_nums[i]] += weights[ant_nums[i]];
    while(dest_weights[ant_nums[i]] < 0)
      dest_weights[ant_nums[i]]+= 360;
    dest_weights[ant_nums[i]] %= 360;
  }
  return 0;
}



int Beamformer::weights_apply(int * weights){
  for(int i = 0; i<ant_amount; i++){
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



int Beamformer::run_beamformer(void){

  char buffer[IO_BUF_SIZE] = {};
  uint16_t tag_id = 0;
  struct average_corr_data data;
  int round = 0;

  Adaptive_beamtrainer BWtrainer(ant_amount);

  std::vector<int> weightVector = BWtrainer.startTraining();
  vector2cur_weights(weightVector);
  weights_apply(cur_weights);

  while(1){

    if(ipc.data_recv(buffer) == -1){
      std::cerr <<"Breaker is activated"<<std::endl;
      break;   
    } 

    memcpy(&data, buffer, sizeof(data));



    /*************************Add algorithm here***************************/



    if(data.successFlag == 1){
      if(!BWtrainer.isTraining()){
        /*
        for(int i = 0; i<ant_amount; i++){
          log<<cur_weights[ant_nums[i]]<< ", ";
        }
        log<<data.avg_corr<<", "<<data.avg_i<<", "<<data.avg_q<<std::endl;
*/

        BWtrainer.startTraining();
      }
      
         for(int i = 0; i<ant_amount; i++){
         log<<cur_weights[ant_nums[i]]<< ", ";
         }
         log<<data.avg_corr<<", "<<data.avg_i<<", "<<data.avg_q<<std::endl;
         

      for(int i = 0; i<16; i++){
        tag_id = tag_id << 1;
        tag_id += data.RN16[i];
      }

      printf("Got RN16 : %x\n",tag_id);
      printf("avg corr : %f\n",data.avg_corr);
      printf("avg iq : %f, %f\n\n",data.avg_i, data.avg_q);

      if(tag_id == PREDFINED_RN16_){
        weightVector = BWtrainer.getRespond(data);
        vector2cur_weights(weightVector);
        if(weights_apply(cur_weights)){
          std::cerr<<"weight apply failed"<<std::endl;
          return 1;
        }
      }

    }else if(data.successFlag == 0){
      printf("Couldn't get RN16\n\n");


      if(!BWtrainer.isTraining()){
        /*
        for(int i = 0; i<ant_amount; i++){
          log<<cur_weights[ant_nums[i]]<< ", ";
        }
        log<<0.0<<", "<<0.0<<", "<<0.0<<std::endl;
*/
        BWtrainer.startTraining();
      }
 
         for(int i = 0; i<ant_amount; i++){
         log<<cur_weights[ant_nums[i]]<< ", ";
         }
         log<<0.0<<", "<<0.0<<", "<<0.0<<std::endl;
         


      weightVector = BWtrainer.cannotGetRespond();
      vector2cur_weights(weightVector);
      if(weights_apply(cur_weights)){
        std::cerr<<"weight apply failed"<<std::endl;
        return 1;
      }
      /*****************************************************************/
    }

    //send ack so that Gen2 program can recognize that the beamforming has been done
    if(ipc.send_ack() == -1){

      break;
    }


    std::cout << "current weight : ";
    for(int i = 0; i<ant_amount; i++){
      std::cout << cur_weights[ant_nums[i]]<< " ";
    }
    std::cout << std::endl;

  }//end of while(1)


  //print wait
  weights_printing(cur_weights);


  return 0;
}

void Beamformer::weights_printing(int * weights){
  for(int i = 0; i<ant_amount; i++){
    std::cout<<"ANT num : "<<ant_nums[i]<<std::endl;
    std::cout<<"Phase : "<<weights[ant_nums[i]]<<std::endl<<std::endl;
  }
}


int Beamformer::start_beamformer(void){
  init_beamformer();

  if(ipc.wait_sync())
    return -1;

  return run_beamformer();
}
