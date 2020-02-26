#ifndef __PHASE_ATTENUATOR_CONTROLLER__
#define __PHASE_ATTENUATOR_CONTROLLER__

#include <iostream>
#include "Vout_controller.h"
#include "csv/csv.h"
#include <complex>
#include <vector>

#define ANT_num   (16)
#define POWER_num (21)
#define DEFAULT_POWER (-6.0)
#define dB2idx(_dB)  (unsigned int)(((_dB) + 13.0)/0.5)
#define DEFAULT_POWER_idx (dB2idx(DEFAULT_POWER))


struct cal_ref{
  std::complex<float> phase;
  float power;
  float ph_V;
  float po_V;
};


class Phase_Attenuator_controller{
  private:
    //Voltage control reference data
    std::vector<struct cal_ref> V_preset[ANT_num][POWER_num];
    int ant_power_setting[ANT_num];
    int index_V_preset[ANT_num][POWER_num][360];

    Vout_controller V;
    int load_cal_data(void);
    int find_matched_preset(int, int, float);
    int set_integer_index(void);

    int voltage_index_search(int, int, int);
    int voltage_index_search(int, int, float);
    int phase_setup(int, int, int);
    int init(void);

  public:
    Phase_Attenuator_controller(void);
    Phase_Attenuator_controller(int);
    Phase_Attenuator_controller(float);
    int phase_control(int, int);
    int phase_control(int, float);
    int phase_control(int, float, int);
    int phase_control(int, float, float);

    int ant_off(int);
    int data_apply();
    void print_integer_index(void);
};

#endif
