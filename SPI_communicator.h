#ifndef __SPI_COMMUNICATOR__
#define __SPI_COMMUNICATOR__

#include <iostream>
#include <errno.h>
#include <unistd.h>
#include <string>
#include <cstring>
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include "Controller_Global.h"


#define MAXIMUM_DATA 256
#define FRAME_SIZE 3


/*
 * UDP communication with RaspberryPi
 */
class SPI_communicator{
  private:
    int sockfd = 0; 
    struct sockaddr_in     servaddr, cliaddr;
    unsigned char buf[MAXIMUM_DATA];
    unsigned int buf_count = 0;
  public:
    /*
     * Initalize UDP socket
     */
    SPI_communicator();
    ~SPI_communicator();

    /*
     * Transmit DAC Control bytes
     */
    int transmit_cmd(const unsigned char * buffer);

    /*
     * Let arduino to set LDAC signal to apply transmitted DAC control bytes
     */
    int data_apply(void);
};

#endif
