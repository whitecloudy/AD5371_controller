#ifndef _ADAPTIVE_BEAMTRAINER_
#define _ADAPTIVE_BEAMTRAINER_

#include <iostream>
#include "Beamformer.h"
#include <armadillo>

class Adaptive_beamtrainer{
  private:
    int antNum;
    int training_count = 0;

    arma::Mat<std::complex<float>> randomWeightMatrix;
    arma::Mat<std::complex<float>> invMatrix;

    arma::Col<std::complex<float>> avgCorrColumn;

  private:
    static float complex2Phase(std::complex<float> complexData);
    static std::complex<float> phase2NormalComplex(float phaseData);

    arma::Row<std::complex<float>> generateRandomWeight(int rowSize);

  public:
    Adaptive_beamtrainer(int ant_num);

    const std::vector<int> getRandomWeight(void);
    const std::vector<int> getRespond(struct average_corr_data recvData);

};

#endif
