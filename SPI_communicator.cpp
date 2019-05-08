#include "SPI_communicator.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>

#define MAXIMUM_BUFFER 256

#define __DEBUG__

const char SERIAL_PORT_1[] = "/dev/ttyACM0" ;
const unsigned char LDAC_bytes[8] = {0x20,0x20,0x20,0x20,0x20,0x20,0x20,0};

SPI_communicator::SPI_communicator(){
  spi_fd = open(SERIAL_PORT_1, O_RDWR | O_NOCTTY | O_NDELAY);
  if(spi_fd==-1) {fprintf(stderr, "serial_port open error!\n"); exit(-1);}
  else
    printf("spi communication success\n");

  std::string input;
  struct termios newtio;
  
  memset(&newtio, 0, sizeof(newtio));
  newtio.c_cflag = B38400;
  newtio.c_cflag |= CS8;
  newtio.c_cflag |= CLOCAL;
  newtio.c_cflag |= CREAD;
  newtio.c_iflag = IGNPAR;
  //   newtio.c_iflag = ICRNL;
  newtio.c_oflag = 0;
  newtio.c_lflag = 0;
  newtio.c_cc[VTIME] = 0;
  newtio.c_cc[VMIN] = 0;

  tcflush(spi_fd, TCIFLUSH);
  tcsetattr(spi_fd, TCSANOW, &newtio);

  int readed = 0;
  void * buffer = malloc(128);
  do{
    sleep(1);
    readed = read(spi_fd,buffer,128);
  }while(readed > 0);
  free(buffer);
}

SPI_communicator::~SPI_communicator(){
  close(spi_fd);
}




int SPI_communicator::transmit_cmd(const unsigned char * spi_bytes){
  unsigned char buffer[8] = {0x21};
  buffer[1] = spi_bytes[0]/16 + 48;
  buffer[2] = (spi_bytes[0]&0x0F) + 48;
  buffer[3] = spi_bytes[1]/16 + 48;
  buffer[4] = (spi_bytes[1]&0x0F) + 48;
  buffer[5] = spi_bytes[2]/16 + 48;
  buffer[6] = (spi_bytes[2]&0x0F) + 48;
#ifdef __DEBUG__

  std::cout<<"Write result : "<<(int)buffer[0]<<" "<<(int)buffer[1]<<" "<<(int)buffer[2]<<" " <<(int)buffer[3]<<" "<<(int)buffer[4]<<" "<<(int)buffer[5]<<" "<<(int)buffer[6]<<" " <<(int)buffer[7]<<std::endl;
#endif
  int result = write(spi_fd, buffer, TX_SIZE);
//  sleep(1);

#ifdef __DEBUG__
  for(int i = 0;i<8;i++)
    buffer[i] = 0;
  int readed = 0;
  int read_count = 0;
  for(read_count = 0;read_count<7;){
    readed = read(spi_fd, (void*)&(buffer[read_count]), 8);
    read_count += readed;
    if(readed!=0)
      std::cout<<"current read_count : "<<read_count <<std::endl;
  }
  std::cout<<"Readed byte : "<<read_count<<std::endl; 
  std::cout<<"Read result : "<<(int)buffer[0]<<" "<<(int)buffer[1]<<" "<<(int)buffer[2]<<" " <<(int)buffer[3]<<" "<<(int)buffer[4]<<" "<<(int)buffer[5]<<" "<<(int)buffer[6]<<" " <<(int)buffer[7]<<std::endl<<std::endl;
  
#endif
  if(result != TX_SIZE)
    return 1;
  return 0;
}

int SPI_communicator::transmit_cmd(std::string buffer){
  int result = write(spi_fd, buffer.c_str(), buffer.length());

#ifdef DEBUG
  std::cout<<buffer<<std::endl;
#endif
 

  if(result != (int)buffer.length())
    return 1;
  return 0;
}


int SPI_communicator::data_apply(){
  int result = write(spi_fd, LDAC_bytes, TX_SIZE);
  std::cout<<"data apply"<<std::endl;
  
  if(result != 7){
    fprintf(stderr,"write result : %d\n",result);
    return 1;
  }
  return 0;
}


