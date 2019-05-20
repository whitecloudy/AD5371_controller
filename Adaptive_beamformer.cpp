#include "Adaptive_beamformer.hpp"
#include <cstring>

#define UNIX_SERVER_SOCKET_NAME "/tmp/epc_gen2_server"

#define DATA  '0'
#define SYNC  '1'
#define ACK   '2'
#define FIN   '3'

#define SOCKET_BUFFER_SIZE 128

Adaptive_beamformer::Adaptive_beamformer(Phase_Attenuator_controller * controller_p, int ant_amount_p, int * ant_num_p){
  this->controller = controller_p;
  this->ant_amount = ant_amount_p;
  this->ant_num = new int[this->ant_amount];
  
  memcpy(ant_num, ant_num_p, sizeof(int)*(this->ant_amount));

  try{
    srv.setup(UNIX_SERVER_SOCKET_NAME);
  }catch(const libsocket::socket_exception& exc){
    std::cerr << exc.mesg;
  }

  client.resize(8);
  recv.resize(128);
}



Adaptive_beamformer::~Adaptive_beamformer(){
  delete this->ant_num;

  try{
    srv.destroy();
  }catch(const libsocket::socket_exception& exc){
    std::cerr << exc.mesg;
  }

}



int Adaptive_beamformer::send_ack(void){
  return srv.sndto("0", 1, client);
}




int Adaptive_beamformer::accept_socket(void){
  try{
    srv.rcvfrom(recv,client);
    if(recv[0] == SYNC) {   //if it is proper SYNC packet
      send_ack();
      return 0;
    }
    else{                       //if it is not
      client.clear();
      return 1;
    }

  }catch(const libsocket::socket_exception& exc){
    std::cerr << exc.mesg;
    return 2;
  }
}





int Adaptive_beamformer::get_data_from_socket(void){
  int data;
  if(srv.rcvfrom(recv, client)<0){
    return 2;
  }

  if(recv[0] == DATA){
    send_ack();
    return 0;
  }else if(recv[0] == FIN){
    send_ack();
    return -1;
  }else
    return 1;

}




int Adaptive_beamformer::send_data_by_socket(std::string buffer){
  int sended = srv.sndto(buffer,client);

  if(sended < 0)
    return 2;

  get_data_from_socket();

  if(recv[0] == ACK)
    return 0;
  else
    return 1;
}



int Adaptive_beamformer::start_beamformer(void){
  while(accept_socket()){
    std::cout<<"wrong sync process"<<std::endl;
  }

  while(1){
    int flag = get_data_from_socket();

  }  
}
