#include <iostream>
#include "Phase_Attenuator_controller.h"
#include "Adaptive_beamformer.h"
#include <cstdlib>
#include <ctime>

#define __NORMAL_BEAMFORMING__

int main(int argc, char ** argv) {
  int ant_amount = argc - 1;
  int * ant_nums = new int[ant_amount];
  Phase_Attenuator_controller ctrl;

  int phase;


  for(int i = 1; i <= ant_amount; i++){
    ant_nums[i-1] = atoi(argv[i]);
    ctrl.ant_off(ant_nums[i-1]);
  }

  ctrl.data_apply();

  //do test beamforming
#ifdef __TEST_BEAMFORMING__
  //first antenna doesn't need phase shift
  ctrl.phase_control(ant_nums[0],0);
  for(int i = 1; i<ant_amount; i++){
    std::cout<<ant_nums[i]<<" antenna"<<std::endl;

    //change phase 10 degree step
    for(phase = 0; phase<360; phase+=10){
      ctrl.phase_control(ant_nums[i], phase);
      ctrl.data_apply();
      std::cout<<phase<<" degree"<<std::endl;
      std::cin.get();    
    }

    //set best phase
    std::cout<<"insert best phase"<<std::endl;
    std::cin>>phase;
    ctrl.phase_control(ant_nums[i], phase);
    ctrl.data_apply();
  }
  //do random phase multi antenna
#elif defined __RANDOM_BEAMFORMING__
  srand(time(NULL));
  for(int i = 0; i<ant_amount; i++){
    phase = rand() % 360;
    std::cout<<ant_nums[i]<<" antenna"<<std::endl;
    std::cout<<phase<<" degree"<<std::endl;
    ctrl.phase_control(ant_nums[i], phase);
    ctrl.data_apply();
  }
#elif defined __NORMAL_BEAMFORMING__
  
  printf("NORMAL BEAMFORMING\n\n");

  Adaptive_beamformer beamformer(&ctrl, ant_amount, ant_nums);

  if(beamformer.start_beamformer()){
    std::cout<< "Error : beamformer has terminated with error"<< std::endl;
    return 1;
  }


#endif

  std::cin.get();

  delete(ant_nums);
  return 0;
}
