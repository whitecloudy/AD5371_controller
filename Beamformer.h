#ifndef __BEAMFORMER__
#define __BEAMFORMER__

#include "Controller_Global.h"
#include "Phase_Attenuator_controller.h"
#include "Adaptive_Beamtrainer.h"
#include "SIC_controller.hpp"
#include "IPC_controller.h"
#include <iostream>
#include <string>
#include <cstdio>
#include <thread>
#include <fstream>


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

class Adaptive_beamtrainer;


class Beamformer{
  private:
    Phase_Attenuator_controller * phase_ctrl;
    IPC_controller ipc;
    std::ofstream log;
    SIC_controller * sic_ctrl;
    Adaptive_beamtrainer * BWtrainer;

    int ant_amount;
    int * ant_nums;
    int cur_weights[ANT_num] = {};
    

  private:
    int weights_addition(int * dest_weights, int * weights0, int * weights1);
    int weights_addition(int * dest_weights, int * weights);

    void weights_printing(int * weights);
    int vector2cur_weights(std::vector<int> weightVector);

    int weights_apply(int * weights);
    int weights_apply(void);

    int init_beamformer(void);
    int run_beamformer(void);
    int calculate_beamforming_weights(void);

    int SIC_port_measure(void);
    int SIC_handler(struct average_corr_data &);
    int Signal_handler(struct average_corr_data &);
    int dataLogging(struct average_corr_data &);

  public:
    Beamformer(Phase_Attenuator_controller * controller, int ant_amount, int * ant_num);
    ~Beamformer();
    int start_beamformer(void);

};

#endif

