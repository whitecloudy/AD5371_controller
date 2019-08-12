#include "Adaptive_Beamtrainer.h"
#include <cmath>
#include <cstdlib>
#include <ctime>

#define BaseNum (32)
#define PI  (3.14159265358979323846)

Adaptive_beamtrainer::Adaptive_beamtrainer(int ant_num){
  this->antNum = ant_num;
  srand(time(NULL));

  avgCorrColumn.set_size(antNum);
  optimalPhaseVector.resize(antNum);
}

/*
 * get phase from complex number
 * 
 * complexData : complex number that you want to get phase
 *
 * <rt> Phase vlaue in degree which is correspond to complex data
 */
float Adaptive_beamtrainer::complex2Phase(std::complex<float> complexData){
  float phase_before_process = (std::arg(complexData)/PI) * 180;
  if(phase_before_process < 0)
    phase_before_process+=360;
  return phase_before_process;
}

/*
 * get normalized complex number corresponding to phase(in degree)
 *
 * phaseData : number of phase in degree
 *
 * <rt> Normalized complex value which is correspond to phase
 */
std::complex<float> Adaptive_beamtrainer::phase2NormalComplex(float phaseData){
  return std::polar((float)1.0, (float)(phaseData*PI)/180);
}


/*make random weight
 *
 * rowSize : size of the vector, normally it is number of antenna
 * 
 * <rt> row of random normal complex
 */
arma::Row<std::complex<float>> Adaptive_beamtrainer::generateRandomWeight(int rowSize){
  arma::Row<std::complex<float>> randomWeight(rowSize);

  for(int i = 0; i<rowSize; i++){
    float phase = (rand()%1440)/4.0;
    randomWeight(i) = phase2NormalComplex(phase);
  }

  return randomWeight;
}


/*
 * Set New RandomWeight on the last matrix row
 */
const std::vector<int> Adaptive_beamtrainer::getRandomWeight(void){
  //make new weight
  
  arma::Row<std::complex<float>> weights = generateRandomWeight(antNum);
  //randomWeightMatrix.insert_rows(training_count, generateRandomWeight(antNum));

  //set vector for phase shift
  std::vector<int> weightVector(antNum);
  for(int i = 0; i< antNum; i++){
    weightVector[i] = complex2Phase(weights(i));
  }

  return weightVector;
}

const std::vector<int> Adaptive_beamtrainer::startTraining(void){
  //reset all the values
  training_count = 0;
  randomWeightMatrix.reset();
  isTraining = true;
  std::cout << "RandomMatrix reset : "<<randomWeightMatrix.n_elem<<std::endl;

  return getRandomWeight();
}


/*
 *  Handle the tag's respond
 */
const std::vector<int> Adaptive_beamtrainer::getRespond(struct average_corr_data recvData){
  return getRandomWeight();
}

/*
 * Handle when the tag does not respond
 */
const std::vector<int> Adaptive_beamtrainer::cannotGetRespond(void){
  return getRandomWeight();
}

/*
 * Tell that the optimal Phase Vector is exist
 */
const bool Adaptive_beamtrainer::isOptimalCalculated(void){
  return !optimalPhaseVector.empty();
}


const std::vector<int> Adaptive_beamtrainer::getOptimalPhaseVector(void){
  return optimalPhaseVector;
}
