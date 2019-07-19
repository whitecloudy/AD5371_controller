#include "Adaptive_beamformer.h"
#include <cstring>
#include <random>
#include <fstream>
#include <sys/time.h>

#define __COLLECT_DATA__
//#define __TIME_STAMP__

#define PREDFINED_RN16_ 0xAAAA

#define Kp 8
#define BETA 0.05
#define SAME_COUNT 3

struct average_corr_data{
  char RN16[16];
  float avg_corr;
};

double normal_random(double mean, double std_dev){
  static std::random_device r;
  static std::default_random_engine generator(r());
  std::normal_distribution<double> distribution(mean, std_dev);

  return distribution(generator);
}



Adaptive_beamformer::Adaptive_beamformer(Phase_Attenuator_controller * controller_p, int ant_amount_p, int * ant_num_p){
  this->phase_ctrl = controller_p;
  this->ant_amount = ant_amount_p;
  this->ant_nums = new int[this->ant_amount];

  memcpy(ant_nums, ant_num_p, sizeof(int)*(this->ant_amount));
}



Adaptive_beamformer::~Adaptive_beamformer(){
  delete this->ant_nums;
}

int Adaptive_beamformer::init_beamformer(void){
  for(int i = 0; i < ant_amount; i++){
    int phase = normal_random(0, 180);
    while(phase < 0)
      phase += 360;
    phase %= 360;

    cur_weights[ant_nums[i]] = phase;
  }

  return weights_apply(cur_weights);
}


int Adaptive_beamformer::weights_addition(int * dest_weights, int * weights0, int * weights1){
  for(int i = 0; i<ant_amount; i++){
    dest_weights[ant_nums[i]] = weights0[ant_nums[i]] + weights1[ant_nums[i]];
    while(dest_weights[ant_nums[i]] < 0)
      dest_weights[ant_nums[i]]+= 360;
    dest_weights[ant_nums[i]] %= 360;
  }

  return 0;
}



int Adaptive_beamformer::weights_addition(int * dest_weights, int * weights){
  for(int i = 0; i<ant_amount; i++){
    dest_weights[ant_nums[i]] += weights[ant_nums[i]];
    while(dest_weights[ant_nums[i]] < 0)
      dest_weights[ant_nums[i]]+= 360;
    dest_weights[ant_nums[i]] %= 360;
  }
  return 0;
}



int Adaptive_beamformer::weights_apply(int * weights){
  for(int i = 0; i<ant_amount; i++){
    phase_ctrl->phase_control(ant_nums[i], weights[ant_nums[i]]);
  }
  return phase_ctrl->data_apply();
}



int Adaptive_beamformer::run_beamformer(void){

  char buffer[IO_BUF_SIZE] = {};

  uint16_t tag_id = 0;

  struct average_corr_data data;

  int round = 0;

  while(1){
    if(ipc.data_recv(buffer) == -1){
      std::cerr <<"Breaker is activated"<<std::endl;
      break;   
    } 

    memcpy(&data, buffer, sizeof(data));

    for(int i = 0; i<16; i++){
      tag_id = tag_id << 1;
      tag_id += data.RN16[i];
    }

    /*************************Add algorithm here***************************/

    //this is just for test
    std::cout << "tag id : "<<tag_id<<std::endl;
    std::cout << "avg corr : "<<data.avg_corr<<std::endl;

    /*****************************************************************/


    //send ack so that Gen2 program can recognize that the beamforming has been done
    if(ipc.send_ack() == -1){
      break;
    }
  }//end of while(1)

  //print wait
  weights_printing(cur_weights);

  return 0;
}

void Adaptive_beamformer::weights_printing(int * weights){
  for(int i = 0; i<ant_amount; i++){
    std::cout<<"ANT num : "<<ant_nums[i]<<std::endl;
    std::cout<<"Phase : "<<weights[ant_nums[i]]<<std::endl<<std::endl;
  }
}


int Adaptive_beamformer::start_beamformer(void){
  init_beamformer();

  if(ipc.wait_sync())
    return -1;

  return run_beamformer();
}
