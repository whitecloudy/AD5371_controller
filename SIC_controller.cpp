#include "SIC_controller.hpp"
#include <cmath>
#include <algorithm>

#define PI (3.1415926535897)
#define Deg2Rad(_num) (float)(_num * (PI / 180))
#define Rad2Deg(_num) (float)(_num / PI * 180.0)

#define dB2Amp(_X) pow(10.0f, _X/20.0f)
#define Amp2dB(_X) 20.0f*log10(_X)
#define _MINdB  (-22.0)
#define _MAXdB  (-3.0)

#define _SIC_DEBUG_

SIC_controller::SIC_controller(std::complex<float> input_ref){
  this->weight_cur = std::polar((float)dB2Amp(SIC_REF_POWER), Deg2Rad(SIC_REF_PHASE)); 
  this->SIC_cha = input_ref/this->weight_cur;
}

int SIC_controller::setCurrentAmp(std::complex<float> amp_Rx){
  std::complex<float> amp_Ant = amp_Rx - SIC_cha * weight_cur;
  std::complex<float> weight_new = -amp_Ant/SIC_cha;

  
#ifdef  _SIC_DEBUG_
  std::cout<<"ANT amp : "<<amp_Ant<<std::endl;
  std::cout<<"SIC channel : "<<SIC_cha<<std::endl;
  std::cout<<"weight_new : "<<weight_new<<std::endl;
  std::cout<<"new dB : "<<Amp2dB(std::abs(weight_new))<<std::endl;
#endif


  float truedB = Amp2dB(std::abs(weight_new));
  float realdB = std::max(std::min(round(truedB*2)/2, _MAXdB),_MINdB);

  weight_new = weight_new / std::abs(weight_new) * (float)dB2Amp(realdB);

  weight_cur = weight_new;

#ifdef  _SIC_DEBUG_
  std::cout<<"weight_true : "<<weight_new<<std::endl;
  std::cout<<"true dB : "<<Amp2dB(std::abs(weight_new))<<std::endl;
  std::cout<<"Expected : "<<amp_Ant + weight_new*SIC_cha<<std::endl;
#endif

  return 0;
}

std::complex<float> SIC_controller::getWeight(void){
  return weight_cur;
}

float SIC_controller::getPower(void){
  return Amp2dB(std::abs(weight_cur));
}

float SIC_controller::getPhase(void){
  return Rad2Deg(std::arg(weight_cur));
}

int SIC_controller::setPower(float power_a){
  this->weight_cur = std::polar((float)dB2Amp(power_a), std::arg(weight_cur));
  return 0;
}

int SIC_controller::setPhase(float phase_a){
  this->weight_cur = std::polar(std::abs(weight_cur), Deg2Rad(phase_a));
  return Rad2Deg(std::arg(weight_cur));
}
