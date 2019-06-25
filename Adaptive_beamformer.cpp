#include "Adaptive_beamformer.h"
#include <cstring>
#include <random>
#include <fstream>

#define __COLLECT_DATA__

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

  //BABF variables
  int Kp_count = 0;
  float Kp_corr_value[Kp+1] = {};
  int Kp_weight_additive[Kp+1][ANT_num] = {};
  int Kp_end_count = 0;

  int round = 0;

#ifdef __COLLECT_DATA__
  std::ofstream log_file("log.txt",std::ofstream::out);
/*
  log_file << "beam weight"<< std::endl;
  for(int i = 0; i < ANT_num; i++){
    log_file << cur_weights[i] << " ";
  }
  log_file << std::endl;
*/

#endif

  while(1){
    if(ipc.send_ack())
      return 1;

    int rt = ipc.data_recv(buffer);
    if(rt == -(IPC_FIN__))
      break;   
    else if(rt < 0){
      std::cout<<"something wrong"<<std::endl;
      break;
    } 

    memcpy(&data, buffer, sizeof(data));

    for(int i = 0; i<16; i++){
      tag_id = tag_id << 1;
      tag_id += data.RN16[i];
    }

    //if tag is our tag
    if(tag_id == PREDFINED_RN16_){
      //handle current Kp
      Kp_corr_value[Kp_count] = data.avg_corr;

      std::cout << "corr : "<< data.avg_corr<<std::endl;


#ifdef __COLLECT_DATA__
  //    log_file <<"corr : "<<data.avg_corr<< std::endl;


#endif

      if(Kp_count > Kp){  //if Kp round is done
        Kp_count = 0;
        int corr_max_index = 0;

        //find maximum correlation Kp index
        for(int i = 0; i<=Kp; i++){
          if(Kp_corr_value[corr_max_index] < Kp_corr_value[i])
            corr_max_index = i;
        }

        if(corr_max_index == 0){
          Kp_end_count += 1;
          if(Kp_end_count >= SAME_COUNT)
            break;
        }else
          Kp_end_count = 0;

        std::cout << "< round "<<round++<<" >"<<std::endl;
        std::cout << "max corr : "<< Kp_corr_value[corr_max_index]<<std::endl;
        std::cout << "max index : "<< corr_max_index<<std::endl<<std::endl;


        //modify current beamweights
        weights_addition(cur_weights, Kp_weight_additive[corr_max_index]);
        weights_apply(cur_weights);

#ifdef __COLLECT_DATA__
        /*
        log_file << "<<<< round : "<<round - 1<< ">>>>" << std::endl;
        log_file << "max index : "<< corr_max_index<<std::endl;
        log_file << "max corr : "<< Kp_corr_value[corr_max_index]<<std::endl;
        log_file << std::endl << std::endl;
        log_file << "beam weight"<< std::endl;
        */
        log_file << round - 1 << ", ";
        for(int i = 0; i < ANT_num; i++){
          log_file << cur_weights[i] << ", ";
        }
        log_file <<Kp_corr_value[corr_max_index];
        log_file << std::endl;


#endif

      }
      else{   //if Kp round is not done yet
        Kp_count+=1;

        //make new addtive of this Kp round
        for(int i = 0; i<ant_amount; i++){
          Kp_weight_additive[Kp_count][ant_nums[i]] = BETA * normal_random(0, 180);
          std::cout<<Kp_weight_additive[Kp_count][ant_nums[i]]<< " ";
        }

        std::cout<<std::endl;

        int tmp_weights[ANT_num];
        weights_addition(tmp_weights, cur_weights, Kp_weight_additive[Kp_count]); 
        weights_apply(tmp_weights);


#ifdef __COLLECT_DATA__
        /*
        log_file << "beam weight"<< std::endl;
        for(int i = 0; i < ANT_num; i++){
          log_file << tmp_weights[i] << " ";
        }
        log_file << std::endl;
*/


#endif


      }
    }
  }//end of while(1)

  log_file.close();

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
  if(ipc.wait_sync())
    return -1;

  init_beamformer();

  return run_beamformer();
}
