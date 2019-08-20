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
  randomWeightMatrix.insert_rows(training_count, generateRandomWeight(antNum));

  //set vector for phase shift
  std::vector<int> weightVector(antNum);
  for(int i = 0; i< antNum; i++){
    weightVector[i] = complex2Phase(randomWeightMatrix(training_count, i));
  }


  if(rank(randomWeightMatrix) < training_count){    //if this is true, that means it is singular matrix
    return cannotGetRespond();
  }else{    //if it is false we simply return
    return weightVector;
  }
}

const std::vector<int> Adaptive_beamtrainer::startTraining(void){
  //reset all the values
  training_count = 0;
  randomWeightMatrix.reset();
  trainingFlag = true;
  std::cout << "RandomMatrix reset : "<<randomWeightMatrix.n_elem<<std::endl;


  //if we have pre-calculated optimal phase, we use it.
  if(isOptimalCalculated()){
    arma::Row<std::complex<float>> optWeightVector(antNum);
    for(int i = 0;i<antNum;i++){
      optWeightVector(i) = phase2NormalComplex(optimalPhaseVector[i]);
    }
    randomWeightMatrix.insert_rows(training_count, optWeightVector);
    return optimalPhaseVector;
  }else{
    optimalPhaseVector.resize(antNum);
    //if we don't have optimal phase yet, we make new random value
    return getRandomWeight();
  }
}


/*
 *  Handle the tag's respond
 */
const std::vector<int> Adaptive_beamtrainer::getRespond(struct average_corr_data recvData){
  if(trainingFlag){
    avgCorrColumn(training_count) = std::complex<float>(recvData.avg_i,recvData.avg_q); //put received amplitude data
    training_count++; //now we can increase training count

    if(randomWeightMatrix.is_square()){ //if it's square that means we are ready to calculate
      trainingFlag = false;   //training is done

      //make inverse Matrix of random Weight Matrix
      invMatrix = inv(randomWeightMatrix);

      arma::Col<std::complex<float>> channelGain = invMatrix * avgCorrColumn;

      //calculate optimal phase vector correspond to channel gain state
      for(int i = 0; i<antNum; i++){
        optimalPhaseVector[i]= (360-complex2Phase(channelGain(i)));
      }
      return optimalPhaseVector;
    }else{
      //still need to training
      return getRandomWeight();
    }
  }else
    return optimalPhaseVector;
}

/*
 * Handle when the tag does not respond
 */
const std::vector<int> Adaptive_beamtrainer::cannotGetRespond(void){
  if(trainingFlag){

    randomWeightMatrix.shed_row(training_count);  //erase last vector
    return getRandomWeight();
  }else
    return optimalPhaseVector;
}



/*
 * Tell that the optimal Phase Vector is exist
 *
 * return true if optimal Phase Vector is exist.
 */
const bool Adaptive_beamtrainer::isOptimalCalculated(void){
  return !optimalPhaseVector.empty();
}

/*
 * Tell that beamtrainer is training
 *
 * return true if it is training
 */
const bool Adaptive_beamtrainer::isTraining(void){
  return trainingFlag;
}




const std::vector<int> Adaptive_beamtrainer::getOptimalPhaseVector(void){
  return optimalPhaseVector;
}
