#ifndef __ADAPTIVE_BEAMFORMER__
#define __ADAPTIVE_BEAMFORMER__

#include "Controller_Global.h"
#include "Phase_Attenuator_controller.h"
#include "SIC_controller.hpp"
#include "IPC_controller.h"
#include <iostream>
#include <string>
#include <cstdio>
#include <thread>

#define _SUCCESS 1
#define _GATE_FAIL 2
#define _PREAMBLE_FAIL 0

struct average_corr_data{
  char successFlag;
  char RN16[16];
  float avg_corr;
  float avg_i;
  float avg_q;
  unsigned int round;
  float cw_i;
  float cw_q;
};


class Beamformer{
  private:
    Phase_Attenuator_controller * phase_ctrl;
    IPC_controller ipc;
    SIC_controller * sic_ctrl;

    int ant_amount;
    int * ant_nums;
    int cur_weights[ANT_num] = {};

  private:
    int weights_addition(int * dest_weights, int * weights0, int * weights1);
    int weights_addition(int * dest_weights, int * weights);

    void weights_printing(int * weights);

    int weights_apply(int * weights);
    int init_beamformer(void);
    int run_beamformer(void);
    int calculate_beamforming_weights(void);

    int SIC_port_measure(void);
    int SIC_handler(struct average_corr_data);
    int Signal_handler(struct average_corr_data);

  public:
    Beamformer(Phase_Attenuator_controller * controller, int ant_amount, int * ant_num);
    ~Beamformer();
    int start_beamformer(void);

};

#endif

