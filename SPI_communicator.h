#include <iostream>
#include <errno.h>
#include <wiringPiSPI.h>
#include <fstream>
#include <unistd.h>
#include <string>

#define CHANNEL 0

#define SPI_DOWN_EDGE 1
#define SPI_UP_EDGE 0

class SPI_communicator{
  private:
    std::ofstream log;
  public:
    SPI_communicator(int channel, int kHz_speed, int mode);
    int transmit(char * buffer, int size);
    ~SPI_communicator();
};
