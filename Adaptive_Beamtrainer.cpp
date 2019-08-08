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
 */
float Adaptive_beamtrainer::complex2Phase(std::complex<float> complexData){
  return (std::arg(complexData)/PI) * 180;
}

/*
 * get normalized complex number corresponding to phase(in degree)
 *
 * phaseData : number of phase in degree
 */
std::complex<float> Adaptive_beamtrainer::phase2NormalComplex(float phaseData){
  return std::polar((float)1.0, (float)(phaseData*PI)/180);
}


/*make random weight
 *
 * rowSize : size of the vector, normally it is number of antenna
 */
arma::Row<std::complex<float>> Adaptive_beamtrainer::generateRandomWeight(int rowSize){
  arma::Row<std::complex<float>> randomWeight(rowSize);

  for(int i = 0; i<rowSize; i++){
    float phase = (rand()%1440)/4.0;
    randomWeight(i) = phase2NormalComplex(phase);
  }

  return randomWeight;
}


const std::vector<int> Adaptive_beamtrainer::getRandomWeight(void){
  //make new weight
  randomWeightMatrix.insert_rows(training_count, generateRandomWeight(antNum));

  //set vector for phase shift
  std::vector<int> weightVector(antNum);
  for(int i = 0; i< antNum; i++){
    weightVector[i] = complex2Phase(randomWeightMatrix[training_count, i]);
  }

  if(rank(randomWeightMatrix) < training_count){    //if this is true, that means we have some singular vector
    randomWeightMatrix.shed_row(training_count);  //erase last vector
    return getRandomWeight();
  }else{    //if it is false we simply return
    return weightVector;
  }
}


const std::vector<int> Adaptive_beamtrainer::getRespond(struct average_corr_data recvData){
  avgCorrColumn(training_count) = std::complex<float>(recvData.avg_i,recvData.avg_q); //put received amplitude data
  training_count++; //now we can increase training count

  if(randomWeightMatrix.is_square()){ //if it's square that means we are ready to calculate
    invMatrix = inv(randomWeightMatrix);



    //reset all the values
    training_count = 0;
    randomWeightMatrix.reset();

  }else{
    return getRandomWeight();
  }
}


