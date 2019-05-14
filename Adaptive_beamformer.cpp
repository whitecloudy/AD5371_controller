#include "Adaptive_beamformer.h"

Adaptive_beamformer::Adaptive_beamformer(Phase_Attenuator_controller * controller_p, int ant_amount_p, int * ant_num_p){
  this->controller = controller_p;
  this->ant_amount = ant_amount_p;
  this->ant_num = new int[this->ant_amount];
  for(int i =0; i<this->ant_amount; i++){
    this->ant_num[i] = ant_num_p[i];
  }
}

int Adaptive_beamformer::start_beamformer(void){

}
