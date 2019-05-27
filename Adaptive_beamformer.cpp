#include "Adaptive_beamformer.h"
#include <cstring>

Adaptive_beamformer::Adaptive_beamformer(Phase_Attenuator_controller * controller_p, int ant_amount_p, int * ant_num_p){
  this->controller = controller_p;
  this->ant_amount = ant_amount_p;
  this->ant_num = new int[this->ant_amount];
  
  memcpy(ant_num, ant_num_p, sizeof(int)*(this->ant_amount));
}



Adaptive_beamformer::~Adaptive_beamformer(){
  delete this->ant_num;
}




int Adaptive_beamformer::start_beamformer(void){
  if(ipc.wait_sync())
    return -1;
  
  char buffer[IO_BUF_SIZE] = {};

  struct average_corr_data{
    char RN16[16];
    float avg_corr;
  } data;

  while(1){
    int rt = ipc.data_recv(buffer);
    if(rt == -IPC_FIN__)
      break;   
    else if(rt < 0){
      std::cout<<"something wrong"<<std::endl;
      break;
    } 

    memcpy(&data, buffer, sizeof(data));

    for(int i = 0; i<16; i++){
      std::cout<<(int)data.RN16[i];
    }
    std::cout <<" : "<<data.avg_corr <<std::endl;


  }


  return 0;
}
