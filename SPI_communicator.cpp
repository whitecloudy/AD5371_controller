#include "SPI_communicator.h"
#include <cstdio>
#include <bcm2835.h>

#define DEBUG false

SPI_communicator::SPI_communicator(int channel, int kHz_speed, int mode){
  std::cout<<"SPI communicator initialize"<<std::endl;
 // spi_fd = wiringPiSPISetupMode(channel, kHz_speed*1000, mode);

  uint16_t divider = 400000/kHz_speed;

  for(int i = 0; i<16; i++){
    if(divider <= 1)
      exit(1);
    if((divider & (0x8000 >> i))){
      divider = (divider & (0x8000 >> i));
      break;
    }
  }

  std::cout<<"divider : "<<divider<<std::endl;

  uint8_t spi_mode;
  if(mode == SPI_DOWN_EDGE)
    spi_mode = BCM2835_SPI_MODE1;
  else if(mode == SPI_UP_EDGE)
    spi_mode = BCM2835_SPI_MODE0;
  else
    exit(1);

  if (!bcm2835_init())
  {
    printf("bcm2835_init failed. Are you running as root??\n");
    exit(1);
  }
  if (!bcm2835_spi_begin())
  {
    printf("bcm2835_spi_begin failed. Are you running as root??\n");
    exit(1);
  }

  bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST); 
  bcm2835_spi_setDataMode(spi_mode);        
  bcm2835_spi_setClockDivider(divider);
  bcm2835_spi_chipSelect(BCM2835_SPI_CS0);       
  bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, LOW);

}

SPI_communicator::~SPI_communicator(){
  bcm2835_spi_end();
	bcm2835_close();
  //close(spi_fd);
}

int SPI_communicator::transmit(char * buffer, int size){
  bcm2835_spi_transfern(buffer, size);

  /*
     int result = write(spi_fd, buffer, size);

     if(DEBUG)
     std::cout<<(int)buffer[0]<<" "<<(int)buffer[1]<<" "<<(int)buffer[2]<<std::endl;
     if(result != size)
     return 1;
     */
  return 0;
}

