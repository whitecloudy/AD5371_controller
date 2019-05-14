#ifndef __ADAPTIVE_BEAMFORMER__
#define __ADAPTIVE_BEAMFORMER__

#include "Controller_Global.h"
#include "Phase_Attenuator_controller.h"
#include <iostream>
#include <cstdio>
#include <thread>

class Adaptive_beamformer{
  private:
    Phase_Attenuator_controller * controller;
    int ant_amount;
    int * ant_num;
    int run_beamformer(void);
    int calculate_beamforming_weights(int * weights);

  public:
    Adaptive_beamformer(Phase_Attenuator_controller * controller, int ant_amount, int * ant_num);
    int start_beamformer(void);

};

#endif

