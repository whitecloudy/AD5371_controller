#ifndef __ADAPTIVE_BEAMFORMER__
#define __ADAPTIVE_BEAMFORMER__

#include "Controller_Global.h"
#include "Phase_Attenuator_controller.h"
#include "IPC_controller.h"
#include <iostream>
#include <string>
#include <cstdio>
#include <thread>


class Adaptive_beamformer{
  private:
    Phase_Attenuator_controller * controller;
    IPC_controller ipc;

    int ant_amount;
    int * ant_num;
    int current_beamweights[ANT_num] = {};

  private:
    int run_beamformer(void);
    int calculate_beamforming_weights(void);

  public:
    Adaptive_beamformer(Phase_Attenuator_controller * controller, int ant_amount, int * ant_num);
    ~Adaptive_beamformer();
    int start_beamformer(void);

};

#endif

