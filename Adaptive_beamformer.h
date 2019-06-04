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
    Phase_Attenuator_controller * phase_ctrl;
    IPC_controller ipc;

    int ant_amount;
    int * ant_nums;
    int cur_weights[ANT_num] = {};

  private:
    int weights_addition(int * dest_weights, int * weights0, int * weights1);
    int weights_addition(int * dest_weights, int * weights);

    int weights_apply(int * weights);
    int init_beamformer(void);
    int run_beamformer(void);
    int calculate_beamforming_weights(void);

  public:
    Adaptive_beamformer(Phase_Attenuator_controller * controller, int ant_amount, int * ant_num);
    ~Adaptive_beamformer();
    int start_beamformer(void);

};

#endif

