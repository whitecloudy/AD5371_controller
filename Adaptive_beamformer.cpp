#include "Adaptive_beamformer.h"
#include <cstring>

struct average_corr_data{
  char RN16[16];
  float avg_corr;
};


Adaptive_beamformer::Adaptive_beamformer(Phase_Attenuator_controller * controller_p, int ant_amount_p, int * ant_num_p){
  this->controller = controller_p;
  this->ant_amount = ant_amount_p;
  this->ant_num = new int[this->ant_amount];

  memcpy(ant_num, ant_num_p, sizeof(int)*(this->ant_amount));
}



Adaptive_beamformer::~Adaptive_beamformer(){
  delete this->ant_num;
}

int Adaptive_beamformer::run_beamformer(void){

  char buffer[IO_BUF_SIZE] = {};

  uint16_t tag_id = 0;
  float corr_value = 0;

  struct average_corr_data data;

  while(1){
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
    corr_value = data.avg_corr;


  }

  return 0;
}


int Adaptive_beamformer::start_beamformer(void){
  if(ipc.wait_sync())
    return -1;
  
  return run_beamformer();
}
