#include "Naive_Beamtrainer.h"

const std::vector<int> Naive_Beamtrainer::getNextWeight(void){
  currentPhaseCount++;
  int step_count = currentPhaseCount;
  if(currentPhaseCount >= (pow(MaxStepNum, (antNum-1)))){  //this means it reaches the end and none of the phase vector able to aquire respond
    currentPhaseVector = optimalPhaseVector;
    isTraining = false;
  }else{
 //   printf("currentPhaseCount: %d\n",currentPhaseCount);
    for(int i = 1; i<antNum; i++){
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

    //add it to avg_amp for averaging 
    avg_amp += std::sqrt(recvData.avg_i * recvData.avg_i + recvData.avg_q * recvData.avg_q);
    avgCalCount++;

    //Check whether it is enough for averaging
    if(avgCalCount < AvgMaxCount){
      return currentPhaseVector;
    }else{
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
    avgCalCount++;
    if(avgCalCount < AvgMaxCount){
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
