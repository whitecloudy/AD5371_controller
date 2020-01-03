#include <iostream>
#include <cstdlib>
#include <string>
#include "Vout_controller.h"

int main(int argc, char * argv[]){

  if(argc != 3)
    return 1;

  Vout_controller vout;
  int vout_number = atoi(argv[1]);
  float volt = atof(argv[2]);
  int modify_result = 0;
  int apply_result = 0;
  for(int i = 0; i<vout_number; i++){
    modify_result = vout.voltage_modify(i, volt);
  }
  apply_result = vout.data_apply();

  std::cout << "offset modify : " << modify_result << std::endl;
  std::cout << "data apply : "<< apply_result << std::endl;

  std::cin >> volt;

  return 0;
}
