#ifndef _ADAPTIVE_BEAMTRAINER_
#define _ADAPTIVE_BEAMTRAINER_

#include <iostream>
#include "Beamformer.h"
#include <armadillo>

class Adaptive_beamtrainer{
  protected:
    int antNum;
    bool isTraining = false;
    bool isOptimal = false;
    float avg_amp = 0.0;
    int initial_phase;

    std::vector<int> optimalPhaseVector;
    std::vector<int> currentPhaseVector;


  protected:
    static float complex2Phase(std::complex<float> complexData);
    static std::complex<float> phase2NormalComplex(float phaseData);

    arma::Row<std::complex<float>> generateRandomWeight(int rowSize);

  public:
    Adaptive_beamtrainer(int ant_num);

    virtual const std::vector<int> startTraining(int initial_phase = 0);

    virtual const std::vector<int> getNextWeight(void);
    virtual const std::vector<int> getRespond(struct average_corr_data recvData);
    virtual const std::vector<int> cannotGetRespond(void);

    const bool isOptimalCalculated(void);
    const std::vector<int> getOptimalPhaseVector(void);
};

#endif
