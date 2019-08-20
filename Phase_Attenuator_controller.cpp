#include "Phase_Attenuator_controller.h"
#include <string>
#include <cmath>


#define ANT1_phase 23
#define ANT2_phase 22
#define ANT3_phase 21
#define ANT4_phase 20
#define ANT5_phase 19
#define ANT6_phase 18
#define ANT7_phase 17
#define ANT8_phase 16
#define ANT9_phase 7
#define ANT10_phase 6
#define ANT11_phase 5
#define ANT12_phase 4
#define ANT13_phase 3
#define ANT14_phase 2
#define ANT15_phase 1
#define ANT16_phase 0

#define ANT1_attenuator 31
#define ANT2_attenuator 30
#define ANT3_attenuator 29
#define ANT4_attenuator 28
#define ANT5_attenuator 27
#define ANT6_attenuator 26
#define ANT7_attenuator 25
#define ANT8_attenuator 24
#define ANT9_attenuator 39
#define ANT10_attenuator 38
#define ANT11_attenuator 37
#define ANT12_attenuator 36
#define ANT13_attenuator 35
#define ANT14_attenuator 34
#define ANT15_attenuator 33
#define ANT16_attenuator 32



const char PHASE[] = {ANT1_phase, ANT2_phase, ANT3_phase, ANT4_phase, ANT5_phase, ANT6_phase, ANT7_phase, ANT8_phase, ANT9_phase, ANT10_phase, ANT11_phase, ANT12_phase, ANT13_phase, ANT14_phase, ANT15_phase, ANT16_phase};
const char ATTENUATOR[] = {ANT1_attenuator, ANT2_attenuator, ANT3_attenuator, ANT4_attenuator, ANT5_attenuator, ANT6_attenuator, ANT7_attenuator, ANT8_attenuator, ANT9_attenuator, ANT10_attenuator, ANT11_attenuator, ANT12_attenuator, ANT13_attenuator, ANT14_attenuator, ANT15_attenuator, ANT16_attenuator};


//#define __DEBUG__

int Phase_Attenuator_controller::load_cal_data(void){
  //loading the calibration data
  for(int i = 0; i<16; i++){
    std::string filename = "calibration_data/ant" + std::to_string(i) + "_cal_data";
    io::CSVReader<4> csv_reader(filename);


    float ph_V, po_V, phase, power;
    int j = 0;
    while(csv_reader.read_row(phase, power, ph_V, po_V)){

      V_preset[i][j].phase = phase;
      V_preset[i][j].power = power;
      V_preset[i][j].ph_V = ph_V;
      V_preset[i][j].po_V = po_V;
      j++;
    }
  }

  return 0;
 
 
}

int preset_finder(struct cal_ref * V_preset, int start, int end, float phase){
  
  int middle = (start + end)/2;

#ifdef __DEBUG__
  std::cout<<start<<", "<<middle<<", "<<end<< " || "<<phase<< " || "<<V_preset[middle].phase<<std::endl;
#endif
  float minus, center, plus;
  int minus_index, center_index, plus_index;

  if(middle==0){    //if middle is zero, left index is most right index because phase is circular
    minus_index = CAL_data_length-1;
    center_index = 0;
    plus_index = 1;
    minus = V_preset[minus_index].phase -360;
    center = V_preset[center_index].phase;
    plus = V_preset[plus_index].phase;
  }else if(middle==CAL_data_length-1){    //if middle is most right index, lef index is zero because phase is circular
    minus_index = middle-1;
    center_index = middle;
    plus_index = 0;
    minus = V_preset[minus_index].phase;
    center = V_preset[center_index].phase;
    plus = V_preset[plus_index].phase + 360;
  }else{
    minus_index = middle-1;
    center_index = middle;
    plus_index = middle+1;
    minus = V_preset[minus_index].phase;
    center = V_preset[center_index].phase;
    plus = V_preset[plus_index].phase;
  }

  if((center < phase) && (plus >= phase)){
    if((phase - center) > (plus - phase))
      return plus_index;
    else
      return center_index;
  }else if((center >= phase) && (minus < phase)){
    if((phase - minus) > (center - phase))
      return center_index;
    else
      return minus_index;
     

  }

  if(V_preset[middle].phase > phase)
    return preset_finder(V_preset, start, middle, phase);
  else
    return preset_finder(V_preset, middle+1, end, phase);
    
    
  /*
  int left_index, middle_index, right_index;
  float left_phase, middle_phase, right_phase;

  for(int i = 0; i<=end ;i++){
    left_index = i-1;
    middle_index = i;
    right_index = i+1;

    middle_phase = V_preset[middle_index].phase;

    if(left_index == -1){
      left_index = end;
      left_phase = V_preset[left_index].phase - 360;
    }else{
      left_phase = V_preset[left_index].phase;
    }

    if(right_index == end+1){
      right_index = 0;
      right_phase = V_preset[right_index].phase + 360;
    }else{
      right_phase = V_preset[right_index].phase;
    }

    if((phase >= left_phase) && (phase < middle_phase)){
      if(std::abs(phase - left_phase) < std::abs(middle_phase - phase))
        return left_index;
      else
        return middle_index;
    }else if((phase >= middle_phase) && (phase < right_phase)){
      if(std::abs(phase - middle_phase) < std::abs(right_phase - phase))
        return middle_index;
      else
        return right_index;
    }
  }

  return -1;
  */
}

int Phase_Attenuator_controller::find_matched_preset(int ant, float phase){
  return preset_finder(V_preset[ant], 0, CAL_data_length-1, phase);
}

int Phase_Attenuator_controller::set_integer_index(void){
  for(int ant = 0; ant < ANT_num; ant++){
    for(int i = 0; i < 360; i++){
      index_V_preset[ant][i] = find_matched_preset(ant, i);
    }
  }
}

int Phase_Attenuator_controller::voltage_index_search(int ant, int phase){
  while(phase < 0)
    phase += 360;

  return index_V_preset[ant][phase%360];
}

int Phase_Attenuator_controller::voltage_index_search(int ant, float phase){
  while(phase > 360)
    phase -= 360.0;
  while(phase < 0)
    phase += 360.0;

  return find_matched_preset(ant, phase);
}

int Phase_Attenuator_controller::phase_setup(int ant, int index){
  int at_result = V.voltage_modify(ATTENUATOR[ant], V_preset[ant][index].po_V);
  int ph_result = V.voltage_modify(PHASE[ant], V_preset[ant][index].ph_V);

  return at_result&&ph_result;
}

int Phase_Attenuator_controller::phase_control(int ant, int phase){
  int index = voltage_index_search(ant, phase);

  //fprintf(stderr,"index : %d\n\n",index);
  //fprintf(stderr,"att voltage = %f\n\n",V_preset[ant][100].po_V);
  //fprintf(stderr,"pha voltage = %f\n\n",V_preset[ant][100].ph_V);
  return phase_setup(ant, index);
}

int Phase_Attenuator_controller::phase_control(int ant, float phase){
  int index = voltage_index_search(ant, phase);
  return phase_setup(ant, index);
}

int Phase_Attenuator_controller::data_apply(void){
  return V.data_apply();
}

int Phase_Attenuator_controller::ant_off(int ant_num){
  return V.voltage_modify(ATTENUATOR[ant_num], 0);
}


Phase_Attenuator_controller::Phase_Attenuator_controller(void){
  if(load_cal_data())
    std::cout<<"Error : Loading calibration data failed"<<std::endl;
  set_integer_index();
}

Phase_Attenuator_controller::Phase_Attenuator_controller(int phase){
  if(load_cal_data())
    std::cout<<"Error : Loading calibration data failed"<<std::endl;
  set_integer_index();

  int result = 0;
  for(int i = 0; i<ANT_num; i++){
    result = result || phase_control(i, phase);
  }
  result = result || data_apply();

  if(result)
    std::cout<<"Error : Phase initalize error"<<std::endl;
}

Phase_Attenuator_controller::Phase_Attenuator_controller(float phase){
  if(load_cal_data())
    std::cout<<"Error : Loading calibration data failed"<<std::endl;
  set_integer_index();

  int result = 0;
  for(int i = 0; i<ANT_num; i++){
    result = result || phase_control(i, phase);
  }
  result = result || data_apply();

  if(result)
    std::cout<<"Error : Phase initalize error"<<std::endl;
}
