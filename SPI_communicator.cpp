#include "SPI_communicator.h"
#include <cstdio>
#include <cstdlib>
#define PORT 10101

#define TX_SIZE 7
#define MAXIMUM_BUFFER 256

#define PHASE_control_cmd 0x21
#define LDAC_cmd  0x20


//#define __DEBUG__
//#define __SERIAL_RETURN__

SPI_communicator::SPI_communicator(){
  if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
    perror("socket creation failed"); 
    exit(EXIT_FAILURE); 
  }  

  memset(&servaddr, 0, sizeof(servaddr)); 
  servaddr.sin_family = AF_INET; 
  servaddr.sin_port = htons(PORT); 
  servaddr.sin_addr.s_addr = inet_addr("192.168.1.3"); 


}

SPI_communicator::~SPI_communicator(){
  close(sockfd);
}




int SPI_communicator::transmit_cmd(const unsigned char * spi_bytes){
  unsigned char buffer[4]= {};
  memcpy(&buffer[1],spi_bytes,3);
  buffer[0] = PHASE_control_cmd;

  int result = sendto(sockfd, buffer, 4, MSG_CONFIRM, (const struct sockaddr *)&servaddr, sizeof(servaddr)); 

  if(result < 0){
    std::cerr<<"send cmd error"<<std::endl;
    return 1;
  }
  
  return 0;
}


int SPI_communicator::data_apply(){
  unsigned char buffer = LDAC_cmd;

  int result = sendto(sockfd, &buffer, 1, MSG_CONFIRM, (const struct sockaddr *)&servaddr, sizeof(servaddr)); 

  if(result < 0){
    std::cerr<<"send LDAC error"<<std::endl;
    return 1;
  }

  std::cout<<"data apply"<<std::endl;

  return 0;
}


