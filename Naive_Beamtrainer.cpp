#include "Naive_Beamtrainer.h"

const std::vector<int> Naive_Beamtrainer::getNextWeight(void){
  if(isFind){
    float phase = (currentPhaseVector[currentAnt] + PhaseStep) % 360;
    //if the phase is same as initial_phase, that means 
    if(phase == initial_phase){
      currentPhaseVector[currentAnt] = optimalPhaseVector[currentAnt];
      currentAnt++;
      //if currentAnt reach antNum, it means training is over
      if(currentAnt >= antNum){
        isTraining = false;
        isOptimal = true;
      }
    }
  }else{
    currentPhaseCount++;
    int step_count = currentPhaseCount;
    for(int i = 1; i<=antNum; i++){
      if(i >= antNum){  //this means it reaches the end and none of the phase vector able to aquire respond
        isTraining = false;
        break;
      }
      currentPhaseVector[i] = (step_count % MaxStepNum)*PhaseStep + initial_phase;
      step_count /= MaxStepNum;
      if(step_count == 0)
        break;
    }
  }
  return currentPhaseVector;
}

const std::vector<int> Naive_Beamtrainer::getRespond(struct average_corr_data recvData){
  if(isTraining){
    //Get respond for the first time
    if(!isFind){
      isFind = true;

      currentAnt = 1;
    }

    //add it to avg_amp for averaging 
    avg_amp += std::sqrt(recvData.avg_i * recvData.avg_i + recvData.avg_q * recvData.avg_q);

    //Check whether it is enough for averaging
    if(avgCalCount < AvgMaxCount){
      avgCalCount++;
      return currentPhaseVector;
    }
    else{
      avg_amp /= AvgMaxCount; //calculate average amp
      if(avg_amp > maxAvgAmp){
        maxAvgAmp = avg_amp;
        optimalPhaseVector = currentPhaseVector;
      }
      avgCalCount = 0;
      avg_amp = 0;
      return getNextWeight();

    }
  }else
    return optimalPhaseVector;
}


const std::vector<int> Naive_Beamtrainer::cannotGetRespond(void){
  if(isTraining){
    if(avgCalCount < AvgMaxCount){
      avgCalCount++;
      return currentPhaseVector;
    }else{
      avg_amp /= AvgMaxCount;
      if(avg_amp > maxAvgAmp){
        maxAvgAmp = avg_amp;
        optimalPhaseVector = currentPhaseVector;
      }

      avgCalCount = 0;
      avg_amp = 0;
      return getNextWeight();
    }
  }else
    return optimalPhaseVector;
}
