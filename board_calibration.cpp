#include <iostream>
#include <cstdlib>
#include <string>
#include "Vout_controller.h"

#define ANT1_phase 23
#define ANT2_phase 22
#define ANT3_phase 21
#define ANT4_phase 20
#define ANT5_phase 19
#define ANT6_phase 18
#define ANT7_phase 17
#define ANT8_phase 16
#define ANT9_phase 7
#define ANT10_phase 6
#define ANT11_phase 5
#define ANT12_phase 4
#define ANT13_phase 3
#define ANT14_phase 2
#define ANT15_phase 1
#define ANT16_phase 0

#define ANT1_attenuator 31
#define ANT2_attenuator 30
#define ANT3_attenuator 29
#define ANT4_attenuator 28
#define ANT5_attenuator 27
#define ANT6_attenuator 26
#define ANT7_attenuator 25
#define ANT8_attenuator 24
#define ANT9_attenuator 39
#define ANT10_attenuator 38
#define ANT11_attenuator 37
#define ANT12_attenuator 36
#define ANT13_attenuator 35
#define ANT14_attenuator 34
#define ANT15_attenuator 33
#define ANT16_attenuator 32

const char PHASE[] = {ANT1_phase, ANT2_phase, ANT3_phase, ANT4_phase, ANT5_phase, ANT6_phase, ANT7_phase, ANT8_phase, ANT9_phase, ANT10_phase, ANT11_phase, ANT12_phase, ANT13_phase, ANT14_phase, ANT15_phase, ANT16_phase};
const char ATTENUATOR[] = {ANT1_attenuator, ANT2_attenuator, ANT3_attenuator, ANT4_attenuator, ANT5_attenuator, ANT6_attenuator, ANT7_attenuator, ANT8_attenuator, ANT9_attenuator, ANT10_attenuator, ANT11_attenuator, ANT12_attenuator, ANT13_attenuator, ANT14_attenuator, ANT15_attenuator, ANT16_attenuator};


int main(int argc, char * argv[]){

  if(argc != 2)
    return 1;

  Vout_controller vout;
  int ant_number = atoi(argv[1]);
  int modify_result = 0;
  int apply_result = 0;
  
  modify_result = vout.voltage_modify(PHASE[ant_number], 0);
  modify_result = vout.voltage_modify(ATTENUATOR[ant_number], 10);
  apply_result = vout.data_apply();

  for(double volt = 0; volt<10.0; volt+=0.1){
    modify_result = vout.voltage_modify(PHASE[ant_number], volt);
    apply_result = vout.data_apply();
    std::cout << "Ant "<<ant_number<<" Phase V : "<<volt<<std::endl;
    sleep(1);
  }

  modify_result = vout.voltage_modify(PHASE[ant_number], 0);
  modify_result = vout.voltage_modify(ATTENUATOR[ant_number], 0);
  apply_result = vout.data_apply();

  for(double volt = 0; volt<10.0; volt+=0.1){
    modify_result = vout.voltage_modify(ATTENUATOR[ant_number], volt);
    apply_result = vout.data_apply();
    std::cout << "Ant "<<ant_number<<" Attenuator V : "<<volt<<std::endl;
    sleep(1);
  }

  modify_result = vout.voltage_modify(PHASE[ant_number], 0);
  modify_result = vout.voltage_modify(ATTENUATOR[ant_number], 0);

  std::cout<< "End of Journey"<<std::endl;

  sleep(1);
 
  return 0;
}
