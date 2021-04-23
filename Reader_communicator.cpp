#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h>
#include "SPI_communicator.h"
#include "GPIO_communicator.h"
#include <signal.h>
#include <bcm2835.h>

#define PORT     10101
#define MAXLINE  (4092)


#define LDAC_pin 5
#define SYNC_pin 6

#define SPI_speed 25000

bool running = true;

void signalHandler(int error){
  bcm2835_spi_end();
	bcm2835_close();

  exit(1);
}

//#define __DEBUG__
int main(int argc, char** argv) { 
  int sockfd;
  int spi_speed = 0;

  struct buf_struct
  {
    uint8_t flag;
    uint8_t data[MAXLINE];
  } buffer;
  struct sockaddr_in servaddr, cliaddr; 

  if(argc == 1)
    spi_speed = SPI_speed;
  else if(argc == 2)
    spi_speed = atoi(argv[1]);
  else
    exit(1);

  std::cout<<"SPI speed : "<<spi_speed<<"kbps"<<std::endl;

  signal(SIGINT, &signalHandler);

  if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
    perror("socket creation failed"); 
    exit(EXIT_FAILURE); 
  } 

  memset(&servaddr, 0, sizeof(servaddr)); 
  memset(&cliaddr, 0, sizeof(cliaddr)); 

  servaddr.sin_family    = AF_INET; // IPv4 
  servaddr.sin_addr.s_addr = INADDR_ANY; 
  servaddr.sin_port = htons(PORT); 
  if (bind(sockfd, (const struct sockaddr *)&servaddr,  
        sizeof(servaddr)) < 0 ) 
  { 
    perror("bind failed"); 
    exit(EXIT_FAILURE); 
  } 

  /****************************************************************************************************
   *  
   *
   * *************************************************************************************************/

  SPI_communicator spi_comm(0, spi_speed, SPI_DOWN_EDGE);
  GPIO_communicator ldac(LDAC_pin, GPIO_UP);

  ////////////////////////////////////////////


  while(running){
    int len = sizeof(struct sockaddr_in);

    int n;

    std::cout <<"Running!"<<std::endl;
    n = recvfrom(sockfd, &buffer, sizeof(struct buf_struct), MSG_WAITALL, (struct sockaddr *) &cliaddr,(socklen_t *) &len); 

#ifdef __DEBUG__
    printf("received packet byte : %d\n",n);
#endif



    if(n<=0 || buffer.flag == 0x01)
      break;

    n -= 1;
    for(int i = 0; i<n; i+=3){
      //    sync.give_signal();
#ifdef __DEBUG__
      printf("sended data : %x %x %x\n",(unsigned int)buffer.data[i],(unsigned int)buffer.data[i+1],(unsigned int)buffer.data[i+2]);
#endif
      spi_comm.transmit(&buffer.data[i], 3);
    }

    ldac.give_signal();
    sendto(sockfd, &n, sizeof(int), MSG_CONFIRM, (const struct sockaddr *)&cliaddr, sizeof(cliaddr));

    std::cout<< n <<std::endl;
  }

  close(sockfd);

  printf("what happend?");

  return 0; 
} 

