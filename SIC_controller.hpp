#ifndef __SIC_CONTROLLER__
#define __SIC_CONTROLLER__

#include <iostream>
#include <complex>

#define SIC_REF_POWER -3.0
#define SIC_REF_PHASE 0

class SIC_controller{
  private:
    std::complex<float> SIC_cha;
    std::complex<float> weight_cur;

  public:
    SIC_controller(std::complex<float>);
    int setCurrentAmp(std::complex<float>);
    std::complex<float> getWeight(void);
    float getPhase(void);
    float getPower(void);
    int setPhase(float);
    int setPower(float);    

};

#endif
