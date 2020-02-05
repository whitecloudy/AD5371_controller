#ifndef __NAIVE_BEAMTRAINER__
#define __NAIVE_BEAMTRAINER__

#include <iostream>
#include "Adaptive_Beamtrainer.h"

#define PhaseStep (5)
#define AvgMaxCount (5)
#define MaxStepNum (360/PhaseStep)


class Naive_Beamtrainer : public Adaptive_beamtrainer{
  private:
    bool isFind = false;
    int avgCalCount = 0;
    int currentAnt = 0;
    int currentPhaseCount = 0;
    float maxAvgAmp = 0.0;
    
  public:
    Naive_Beamtrainer(int ant_num) : Adaptive_beamtrainer(ant_num){}
    const std::vector<int> getNextWeight(void);
    const std::vector<int> getRespond(struct average_corr_data recvData);
    const std::vector<int> cannotGetRespond(void);

};

#endif
