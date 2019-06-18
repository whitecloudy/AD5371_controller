#include "Adaptive_beamformer.h"
#include <cstring>
#include <random>
#include <fstream>

#define PORT 33333
#define MAX_BUFFER 2048
#define SA struct sockaddr 

#define __COLLECT_DATA__

#define PREDFINED_RN16_ 0xAAAA

#define Kp 8
#define BETA 0.05
#define SAME_COUNT 3

struct average_corr_data{
  char RN16[16];
  float avg_corr;
};

Adaptive_beamformer::Adaptive_beamformer(Phase_Attenuator_controller * controller_p, int ant_amount_p, int * ant_num_p){
  this->phase_ctrl = controller_p;
  this->ant_amount = ant_amount_p;
  this->ant_nums = new int[this->ant_amount];

  memcpy(ant_nums, ant_num_p, sizeof(int)*(this->ant_amount));

  set_tcp();
}



Adaptive_beamformer::~Adaptive_beamformer(){
  delete this->ant_nums;

  std::cout<<"closed"<<std::endl;
  close(connfd);
  close(sockfd);
}

int Adaptive_beamformer::weights_addition(int * dest_weights, int * weights0, int * weights1){
  for(int i = 0; i<ant_amount; i++){
    dest_weights[ant_nums[i]] = weights0[ant_nums[i]] + weights1[ant_nums[i]];
    while(dest_weights[ant_nums[i]] < 0)
      dest_weights[ant_nums[i]]+= 360;
    dest_weights[ant_nums[i]] %= 360;
  }

  return 0;
}



int Adaptive_beamformer::weights_addition(int * dest_weights, int * weights){
  for(int i = 0; i<ant_amount; i++){
    dest_weights[ant_nums[i]] += weights[ant_nums[i]];
    while(dest_weights[ant_nums[i]] < 0)
      dest_weights[ant_nums[i]]+= 360;
    dest_weights[ant_nums[i]] %= 360;
  }
  return 0;
}



int Adaptive_beamformer::weights_apply(int * weights){
  for(int i = 0; i<ant_amount; i++){
    phase_ctrl->phase_control(ant_nums[i], weights[ant_nums[i]]);
  }
  return phase_ctrl->data_apply();
}



int Adaptive_beamformer::run_beamformer(void){

  char buffer[IO_BUF_SIZE] = {};

  uint16_t tag_id = 0;

  struct average_corr_data data;

  int round = 0;

#ifdef __COLLECT_DATA__
  std::ofstream log_file("log.txt",std::ofstream::app);
#endif


  bool redo_flag = true;
  while(1){
    if(redo_flag){
      get_weights(cur_weights);
      weights_apply(cur_weights);
      redo_flag = false;
    }
    if(ipc.send_ack())
      return 1;

    int rt = ipc.data_recv(buffer);
    if(rt < 0){
      std::cout << rt <<std::endl;
      ipc.wait_sync();
      continue;
    }

    std::cout << rt <<std::endl;


    memcpy(&data, buffer, sizeof(data));

    for(int i = 0; i<16; i++){
      tag_id = tag_id << 1;
      tag_id += data.RN16[i];
    }

    //if tag is our tag
    if(tag_id == PREDFINED_RN16_){
      set_correlation(data.avg_corr);

#ifdef __COLLECT_DATA__
      log_file << round++ <<", ";
      for(int i = 0; i<ANT_num; i++){
        log_file << cur_weights[i] << ", ";
      }
      log_file << data.avg_corr <<std::endl;

#endif

      redo_flag = true;
    }else{
      std::cout << "wrong data"<<std::endl;
      std::cout << tag_id<<std::endl;
      //      set_correlation(0.0);

#ifdef __COLLECT_DATA__
      //    log_file << round++ <<", ";
      //  for(int i = 0; i<ANT_num; i++){
      //    log_file << cur_weights[i] << ", ";
      //  }
      //  log_file << 0 <<std::endl;

#endif
    }

  }//end of while(1)

#ifdef __COLLECT_DATA__
  log_file.close();
#endif

  weights_printing(cur_weights);

  return 0;
}

void Adaptive_beamformer::weights_printing(int * weights){
  for(int i = 0; i<ant_amount; i++){
    std::cout<<"ANT num : "<<ant_nums[i]<<std::endl;
    std::cout<<"Phase : "<<weights[ant_nums[i]]<<std::endl<<std::endl;
  }
}


int Adaptive_beamformer::start_beamformer(void){
  if(ipc.wait_sync())
    return -1;

  return run_beamformer();
}


void Adaptive_beamformer::set_tcp(){
  sockfd = socket(AF_INET, SOCK_STREAM, 0); 
  if (sockfd == -1) { 
    printf("socket creation failed...\n"); 
    exit(0); 
  } 
  else
    printf("Socket successfully created..\n"); 
  bzero(&servaddr, sizeof(servaddr)); 

  servaddr.sin_family = AF_INET; 
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
  servaddr.sin_port = htons(PORT); 


  if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) { 
    printf("socket bind failed...\n"); 
    exit(0); 
  } 
  else
    printf("Socket successfully binded..\n"); 

  if ((listen(sockfd, 5)) != 0) { 
    printf("Listen failed...\n"); 
    exit(0); 
  } 
  else
    printf("Server listening..\n"); 
  len = sizeof(cli); 

  connfd = accept(sockfd, (SA*)&cli, (socklen_t*)&len); 
  if (connfd < 0) { 
    printf("server acccept failed...\n"); 
    exit(0); 
  } 
  else
    printf("server acccept the client...\n"); 

}

int Adaptive_beamformer::get_weights(int * weights){
  int n;
  char buf[MAX_BUFFER] = {};

  n = read(connfd, buf, sizeof(buf));
  buf[n] = NULL;

  rapidjson::Document doc;

  if(doc.Parse(buf).HasParseError()){
    std::cerr<<"Parse failed : "<<buf<<std::endl;
    exit(1);
  }

  assert(doc["w1"].IsString());
  assert(doc["w2"].IsString());
  assert(doc["w3"].IsString());

  weights[ant_nums[0]] = std::stof(doc["w1"].GetString());
  weights[ant_nums[1]] = std::stof(doc["w2"].GetString());
  weights[ant_nums[2]] = std::stof(doc["w3"].GetString());

  return 0;
}

int Adaptive_beamformer::set_correlation(float corr_value){
  const char* json = "{\"correlation\":0.0}";
  rapidjson::Document doc1;

  doc1.Parse(json);

  doc1["correlation"] = corr_value;

  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  doc1.Accept(writer);

  write(connfd, buffer.GetString(), strlen(buffer.GetString()));

  return 0;
}
