#include "SPI_communicator.h"
#include <cstdio>
#include <cstdlib>
#define PORT 10101

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

  memset(&buf, 0, sizeof(MAXIMUM_DATA));
  
}

SPI_communicator::~SPI_communicator(){
  sendto(sockfd,buf,0,MSG_CONFIRM,(const struct sockaddr *)&servaddr, sizeof(servaddr));
  close(sockfd);
}




int SPI_communicator::transmit_cmd(const unsigned char * spi_bytes){
  memcpy(&buf[buf_count*FRAME_SIZE],spi_bytes,3);
  buf_count++;

 // int result = sendto(sockfd, buffer, 4, MSG_CONFIRM, (const struct sockaddr *)&servaddr, sizeof(servaddr)); 

  return 0;
}


int SPI_communicator::data_apply(){
  int result = sendto(sockfd, buf, buf_count*FRAME_SIZE, MSG_CONFIRM, (const struct sockaddr *)&servaddr, sizeof(servaddr));

  if(result < 0){
    std::cerr<<"send SPI data error"<<std::endl;
    return 1;
  }

  buf_count = 0;

  std::cout<<"data applied"<<std::endl;

  return 0;
}


