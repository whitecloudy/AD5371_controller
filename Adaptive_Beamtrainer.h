#ifndef _ADAPTIVE_BEAMTRAINER_
#define _ADAPTIVE_BEAMTRAINER_

#include <iostream>
#include "Beamformer.h"
#include <armadillo>

class Adaptive_beamtrainer{
  private:
    int antNum;
    bool trainingFlag = false;
    int training_count = 0;

    arma::Mat<std::complex<float>> trainingWeightMatrix;
    arma::Mat<std::complex<float>> invMatrix;

    arma::Col<std::complex<float>> avgCorrColumn;

    std::vector<std::vector<int>> optimalPhaseVectors;

  private:
    static float complex2Phase(std::complex<float> complexData);
    arma::Row<std::complex<float>> vector2row(std::vector<int> weightVector);
    static std::complex<float> phase2NormalComplex(float phaseData);

    arma::Row<std::complex<float>> generateRandomWeight(int rowSize);

  public:
    Adaptive_beamtrainer(int ant_num);

    const std::vector<int> startTraining(void);

    const std::vector<int> getRandomWeight(void);
    const std::vector<int> getRespond(struct average_corr_data recvData);
    const std::vector<int> cannotGetRespond(void);

    const bool isOptimalCalculated(void);
    const bool isTraining(void);

    const std::vector<int> getOptimalPhaseVector(void);
};

#endif
