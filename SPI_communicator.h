#ifndef __SPI_COMMUNICATOR__
#define __SPI_COMMUNICATOR__

#include <iostream>
#include <errno.h>
#include <unistd.h>
#include <string>
#include <termios.h>
#include <fcntl.h>
#include <cstring>
#include "Controller_Global.h"
/*
 * Serial communicator with Ardunio
 */
class SPI_communicator{
  private:
    int spi_fd = 0; 
  public:
    /*
     * Initalize Seiral communicate file discripter
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
