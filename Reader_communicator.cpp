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

#define PORT     10101
#define MAXLINE 256


#define LDAC_pin 5
#define SYNC_pin 6

#define SPI_speed 40000


//#define __DEBUG__


int main() { 
  int sockfd;
  unsigned char buffer[MAXLINE]; 
  struct sockaddr_in servaddr, cliaddr; 

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

  SPI_communicator spi_comm(0, SPI_speed, SPI_DOWN_EDGE);
  GPIO_communicator ldac(LDAC_pin, GPIO_UP);
  GPIO_communicator sync(SYNC_pin, GPIO_DOWN);

  ////////////////////////////////////////////


  while(1){
    int len, n;
    n = recvfrom(sockfd, buffer, MAXLINE, MSG_WAITALL, (struct sockaddr *) &cliaddr,(socklen_t *) &len); 

    if(n<=0)
      break;
#ifdef __DEBUG__
    printf("received packet byte : %d\n",n);
#endif


    for(int i = 0; i<n; i+=3){

      sync.give_signal();
      spi_comm.transmit(&buffer[i], 3);
      


#ifdef __DEBUG__
      printf("sended data : %d %d %d\n",buffer[i],buffer[i+1],buffer[i+2]);
#endif
    }


    ldac.give_signal();
  }

close(sockfd);


return 0; 
} 

