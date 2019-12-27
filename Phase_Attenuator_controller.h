#ifndef __PHASE_ATTENUATOR_CONTROLLER__
#define __PHASE_ATTENUATOR_CONTROLLER__

#include <iostream>
#include "Vout_controller.h"
#include "csv/csv.h"

#define ANT_num 16
#define CAL_data_length 398
struct cal_ref{
  float phase;
  float power;
  float ph_V;
  float po_V;
};


class Phase_Attenuator_controller{
  private:
    //Voltage control reference data
    struct cal_ref V_preset[ANT_num][CAL_data_length];
    int index_V_preset[ANT_num][360];

    Vout_controller V;
    int load_cal_data(void);
    int find_matched_preset(int, float);
    int set_integer_index(void);

    int voltage_index_search(int, int);
    int voltage_index_search(int, float);

    int phase_setup(int, int);

  public:
    Phase_Attenuator_controller(void);
    Phase_Attenuator_controller(int);
    Phase_Attenuator_controller(float);
    int phase_control(int, int);
    int phase_control(int, float);
    int ant_off(int);
    int data_apply();
    void print_integer_index(void);
};

#endif
