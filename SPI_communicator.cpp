#include "SPI_communicator.h"
#include <cstdio>

#define MAXIMUM_BUFFER 256

const char SERIAL_PORT_1[] = "/dev/ttyACM0" ;
const unsigned char LDAC_bytes[] = {0xFF, '\0'};

SPI_communicator::SPI_communicator(){
  spi_fd = open(SERIAL_PORT_1, O_RDWR | O_NOCTTY | O_NDELAY);
  if(spi_fd==-1) {fprintf(stderr, "serial_port open error!\n"); exit(-1);}

  std::string input;
  struct termios newtio;

  memset(&newtio, 0, sizeof(newtio));

  newtio.c_cflag = B230400;
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


}

SPI_communicator::~SPI_communicator(){
  close(spi_fd);
}

int SPI_communicator::transmit_cmd(const unsigned char * spi_bytes){
  const unsigned char buffer[4] = {0x00, spi_bytes[0], spi_bytes[1], spi_bytes[2]};
  int result = write(spi_fd, buffer, TX_SIZE);

#ifdef DEBUG
  std::cout<<(int)buffer[0]<<" "<<(int)buffer[1]<<" "<<(int)buffer[2]<<std::endl;
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
  int result = write(spi_fd, LDAC_bytes, 2);

  if(result != 2)
    return 1;
  return 0;
}


