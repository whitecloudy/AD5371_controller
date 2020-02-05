#ifndef _ADAPTIVE_BEAMTRAINER_
#define _ADAPTIVE_BEAMTRAINER_

#include <iostream>
#include "Beamformer.h"
#include <armadillo>

class Adaptive_beamtrainer{
  protected:
    int antNum;
    bool isTraining = false;
    bool isRespond = false;
    bool isOptimal = false;
    float avg_amp = 0.0;
    int currentTrainingAnt = 0;
    int phaseRoundCount = 0;
    int bestPhase = 0;
    double bestPhasePower = 0.0;
    int initial_phase;

    std::vector<int> optimalPhaseVector;
    std::vector<int> currentPhaseVector;


  protected:
    static float complex2Phase(std::complex<float> complexData);
    static std::complex<float> phase2NormalComplex(float phaseData);
    static const std::vector<int> row2Vector(arma::Row<std::complex<float>> row);

    arma::Row<std::complex<float>> generateRandomWeight(int rowSize);
    virtual const std::vector<int> getNextWeight(void);

  public:
    Adaptive_beamtrainer(int ant_num);

    virtual const std::vector<int> startTraining(int initial_phase = 0);

    virtual const std::vector<int> getRespond(struct average_corr_data recvData);
    virtual const std::vector<int> cannotGetRespond(void);

    const bool isOptimalCalculated(void);
    const std::vector<int> getOptimalPhaseVector(void);
};

#endif
