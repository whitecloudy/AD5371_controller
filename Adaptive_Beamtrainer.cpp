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

arma::Row<std::complex<float>> Adaptive_beamtrainer::vector2row(std::vector<int> weightVector){
  arma::Row<std::complex<float>> weightRow(antNum);
  for(int i = 0; i<antNum; i++){
    weightRow(i) = phase2NormalComplex(weightVector[i]);
  }

  return weightRow;
}


/*
 * Set New TrainingWeight on the last matrix row
 */
const std::vector<int> Adaptive_beamtrainer::getTrainingWeight(void){
  std::vector<int> weightVector(antNum);

  //if we have a trained weight Vector;
  if(training_count < optimalPhaseVectors.size()){
    weightVector = optimalPhaseVectors[optimalPhaseVectors.size() - training_count - 1];
    trainingWeightMatrix.insert_rows(training_count, vector2row(weightVector));
  }else{
    //make new weight
    trainingWeightMatrix.insert_rows(training_count, generateRandomWeight(antNum));

    //set vector for phase shift
    for(int i = 0; i< antNum; i++){
      weightVector[i] = complex2Phase(trainingWeightMatrix(training_count, i));
    }
  }


  if(rank(trainingWeightMatrix) < training_count){    //if this is true, that means it is singular matrix
    return cannotGetRespond();
  }else{    //if it is false we simply return
    return weightVector;
  }
}


/*
 * Set New RandomWeight on the last matrix row
 */
const std::vector<int> Adaptive_beamtrainer::getRandomWeight(void){
    std::vector<int> weightVector(antNum);

    //make new weight
    trainingWeightMatrix.insert_rows(training_count, generateRandomWeight(antNum));

    //set vector for phase shift
    for(int i = 0; i< antNum; i++){
	    weightVector[i] = complex2Phase(trainingWeightMatrix(training_count, i));
    }


    if(rank(trainingWeightMatrix) < training_count){    //if this is true, that means it is singular matrix
	    return cannotGetRespond();
    }else{    //if it is false we simply return
	    return weightVector;
    }
}

const std::vector<int> Adaptive_beamtrainer::startTraining(void){
	//reset all the values
	training_count = 0;
	trainingWeightMatrix.reset();
	trainingFlag = true;
	std::cout << "RandomMatrix reset : "<<trainingWeightMatrix.n_elem<<std::endl;

	//if we don't have optimal phase yet, we make new random value
	return getTrainingWeight();
}


/*
 *  Handle the tag's respond
 */
const std::vector<int> Adaptive_beamtrainer::getRespond(struct average_corr_data recvData){
	if(trainingFlag){
		avgCorrColumn(training_count) = std::complex<float>(recvData.avg_i,recvData.avg_q); //put received amplitude data
		training_count++; //now we can increase training count

		if(trainingWeightMatrix.is_square()){ //if it's square that means we are ready to calculate
			std::vector<int> optimalPhaseVector(antNum);

			trainingFlag = false;   //training is done

			//make inverse Matrix of random Weight Matrix
			invMatrix = inv(trainingWeightMatrix);

			arma::Col<std::complex<float>> channelGain = invMatrix * avgCorrColumn;

			//calculate optimal phase vector correspond to channel gain state
			for(int i = 0; i<antNum; i++){
				optimalPhaseVector[i]= (360-complex2Phase(channelGain(i)));
			}
			optimalPhaseVectors.push_back(optimalPhaseVector);
			return optimalPhaseVector;
		}else{
			//still need to training
			return getTrainingWeight();
		}
	}else
		return optimalPhaseVectors.back();
}

/*
 * Handle when the tag does not respond
 */
const std::vector<int> Adaptive_beamtrainer::cannotGetRespond(void){
	if(trainingFlag){
		trainingWeightMatrix.shed_row(training_count);  //erase last vector
		return getRandomWeight();
	}else
		return optimalPhaseVectors.back();
}



/*
 * Tell that the optimal Phase Vector is exist
 *
 * return true if optimal Phase Vector is exist.
 */
const bool Adaptive_beamtrainer::isOptimalCalculated(void){
	return !optimalPhaseVectors.empty();
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
	return optimalPhaseVectors.back();
}
