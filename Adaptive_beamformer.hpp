#ifndef __ADAPTIVE_BEAMFORMER__
#define __ADAPTIVE_BEAMFORMER__

#include "Controller_Global.h"
#include "Phase_Attenuator_controller.h"
#include <iostream>
#include <string>
#include <cstdio>
#include <thread>
#include <libsocket/unixserverdgram.hpp>
#include <libsocket/exception.hpp>


class Adaptive_beamformer{
  private:
    Phase_Attenuator_controller * controller;

    int ant_amount;
    int * ant_num;
    int current_beamweights[ANT_num] = {};
    libsocket::unix_dgram_server srv;
    std::string client;
    std::string recv;


  private:
    int run_beamformer(void);
    int calculate_beamforming_weights(int * weights);

  private:
    int accept_socket(void);
    int get_data_from_socket(void);
    int send_data_by_socket(std::string buffer);
    int send_ack(void);

  public:
    Adaptive_beamformer(Phase_Attenuator_controller * controller, int ant_amount, int * ant_num);
    ~Adaptive_beamformer();
    int start_beamformer(void);

};

#endif

