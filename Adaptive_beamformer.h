#ifndef __ADAPTIVE_BEAMFORMER__
#define __ADAPTIVE_BEAMFORMER__

#include "Controller_Gloabal.h"
#include "Phase_Attenuator_controller.h"
#include <iostream>
#include <cstdio>


class Adaptive_beamformer{
  private:
    Phase_Attenuator_controller * controller;
    int ant_amount;
    int * ant_num;



  public:
    Adaptive_beamformer(Phase_Attenuator_controller * controller, int ant_amount, int * ant_num);

};


#endif

