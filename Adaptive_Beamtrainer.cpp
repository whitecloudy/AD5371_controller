#include "Adaptive_Beamtrainer.h"
#include <cmath>
#include <cstdlib>
#include <ctime>

#define BaseNum (32)
#define PI  (3.14159265358979323846)

Adaptive_beamtrainer::Adaptive_beamtrainer(int ant_num){
  this->antNum = ant_num;
  srand(time(NULL));

  optimalPhaseVector.resize(antNum);
  optimalPhaseVector.assign(antNum, 0);

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
 * make row to vector
 *
 *
 *
 */
const std::vector<int> Adaptive_beamtrainer::row2Vector(arma::Row<std::complex<float>> row){
  int size = row.n_cols;
  std::vector<int> rowVector(size, 0);

  for(int i = 0; i< size; i++){
    rowVector[i] = complex2Phase(row(i));
  }

  return rowVector;
}




/*
 * Set New RandomWeight on the last matrix row
 */
const std::vector<int> Adaptive_beamtrainer::getNextWeight(void){
  if(phaseRoundCount < BaseNum){
    currentPhaseVector[currentTrainingAnt] = (currentPhaseVector[currentTrainingAnt] + 360/BaseNum)%360;
    phaseRoundCount++;
  }else{
    currentPhaseVector[currentTrainingAnt] = bestPhase;
    currentTrainingAnt++;
    phaseRoundCount = 0;

    if(currentTrainingAnt >= antNum){
      optimalPhaseVector = currentPhaseVector;
      isTraining = false;
    }
  }

  return currentPhaseVector;
}

const std::vector<int> Adaptive_beamtrainer::startTraining(int initial_phase){
  //reset all the values
  isTraining = true;

  currentPhaseVector = row2Vector(generateRandomWeight(antNum));

  currentPhaseVector.assign(antNum, initial_phase);

  return currentPhaseVector;
}


/*
 *  Handle the tag's respond
 */
const std::vector<int> Adaptive_beamtrainer::getRespond(struct average_corr_data recvData){
  if(isTraining){
    if(!isRespond){
      isRespond = true;
      currentTrainingAnt = 0;
      phaseRoundCount = 0;
    }else{
      double currentPhasePower = pow(recvData.avg_i,2)+pow(recvData.avg_q,2);
      if(currentPhasePower>bestPhasePower){
        bestPhasePower = currentPhasePower;
        bestPhase = currentPhaseVector[currentTrainingAnt];
      }
      getNextWeight();
    }
    return currentPhaseVector;
  }
  else
    return optimalPhaseVector;
}

/*
 * Handle when the tag does not respond
 */
const std::vector<int> Adaptive_beamtrainer::cannotGetRespond(void){
  if(isTraining){
    if(isRespond){
      getNextWeight();
    }else{
      currentPhaseVector = row2Vector(generateRandomWeight(antNum));
    }
    return currentPhaseVector;
  }else
    return optimalPhaseVector;
}

/*
 * Tell that the optimal Phase Vector is exist
 */
const bool Adaptive_beamtrainer::isOptimalCalculated(void){
  return isOptimal;
}


const std::vector<int> Adaptive_beamtrainer::getOptimalPhaseVector(void){
  return optimalPhaseVector;
}
