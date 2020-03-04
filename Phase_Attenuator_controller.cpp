#include "Phase_Attenuator_controller.h"
#include <string>
#include <cmath>


enum VOUTNUM
{
  ANT1_phase = 23,
  ANT2_phase = 22,
  ANT3_phase = 21,
  ANT4_phase = 20,
  ANT5_phase = 19,
  ANT6_phase = 18,
  ANT7_phase = 17,
  ANT8_phase = 16,
  ANT9_phase = 7,
  ANT10_phase = 6,
  ANT11_phase = 5,
  ANT12_phase = 4,
  ANT13_phase = 3,
  ANT14_phase = 2,
  ANT15_phase = 1,
  ANT16_phase = 0,
  ANT1_attenuator = 31,
  ANT2_attenuator = 30,
  ANT3_attenuator = 29,
  ANT4_attenuator = 28,
  ANT5_attenuator = 27,
  ANT6_attenuator = 26,
  ANT7_attenuator = 25,
  ANT8_attenuator = 24,
  ANT9_attenuator = 39,
  ANT10_attenuator = 38,
  ANT11_attenuator = 37,
  ANT12_attenuator = 36,
  ANT13_attenuator = 35,
  ANT14_attenuator = 34,
  ANT15_attenuator = 33,
  ANT16_attenuator = 32
};


#define PI (3.1415926535897)

#define Deg2Rad(_num) (float)(_num * (PI / 180))

const char PHASE[] = {ANT1_phase, ANT2_phase, ANT3_phase, ANT4_phase, ANT5_phase, ANT6_phase, ANT7_phase, ANT8_phase, ANT9_phase, ANT10_phase, ANT11_phase, ANT12_phase, ANT13_phase, ANT14_phase, ANT15_phase, ANT16_phase};
const char ATTENUATOR[] = {ANT1_attenuator, ANT2_attenuator, ANT3_attenuator, ANT4_attenuator, ANT5_attenuator, ANT6_attenuator, ANT7_attenuator, ANT8_attenuator, ANT9_attenuator, ANT10_attenuator, ANT11_attenuator, ANT12_attenuator, ANT13_attenuator, ANT14_attenuator, ANT15_attenuator, ANT16_attenuator};
const std::string POWER_PRESET[] = {"-13.0", "-12.5", "-12.0", "-11.5", "-11.0", "-10.5", "-10.0", "-9.5", "-9.0", "-8.5", "-8.0", "-7.5", "-7.0", "-6.5", "-6.0", "-5.5", "-5.0", "-4.5", "-4.0", "-3.5", "-3.0"};




//#define __DEBUG__

int Phase_Attenuator_controller::load_cal_data(void){
  //loading the calibration data
  
  std::cout << "Reading calibration data.....";
  for(int power_idx = 0;  power_idx < POWER_num; power_idx++){
    for(int i = 0; i<ANT_num; i++){
      std::string filename = "calibration_data/ant" + std::to_string(i) +
        "_" + POWER_PRESET[power_idx] + "dB" + 
        "_cal_data";
      io::CSVReader<4> csv_reader(filename);


      float ph_V, po_V, phase, power;
      while(csv_reader.read_row(phase, power, ph_V, po_V)){
        struct cal_ref v_data;
        v_data.phase = std::polar(1.0f, Deg2Rad(phase));
        v_data.power = power;
        v_data.ph_V = ph_V;
        v_data.po_V = po_V;

        V_preset[i][power_idx].push_back(v_data);
      }
    }
  }
  std::cout << "Done"<<std::endl;

  return 0;
}


int preset_finder(std::vector<struct cal_ref> V_preset, float phase){
  int vec_len = V_preset.size();  
  std::complex<float> phase_com = std::polar(1.0f, Deg2Rad(phase));

  int min_idx = 0;
  double min_value = 999999.9;

  for(int i = 0; i< vec_len; i++){
    if(std::abs(V_preset[i].phase - phase_com) < min_value){
      min_idx = i;
      min_value = std::abs(V_preset[i].phase - phase_com);
    }
  }

  return min_idx;
}

int preset_finder(std::vector<struct cal_ref> V_preset, int start, int end, float phase){
  return preset_finder(V_preset, phase);
}


int Phase_Attenuator_controller::find_matched_preset(int ant, int power, float phase){
  return preset_finder(V_preset[ant][power], phase);
}


int Phase_Attenuator_controller::set_integer_index(void){
  std::cerr << "Preparing integer indexes...";
  for(int power_idx = 0; power_idx < POWER_num; power_idx++){
    for(int ant = 0; ant < ANT_num; ant++){
      for(int i = 0; i < 360; i++){
        index_V_preset[ant][power_idx][i] = find_matched_preset(ant, power_idx ,i);
      }
    }
  }
  std::cout << "Done"<<std::endl;
}


int Phase_Attenuator_controller::voltage_index_search(int ant, int power_idx, int phase){
  while(phase < 0)
    phase += 360;

  return index_V_preset[ant][power_idx][phase%360];
}

int Phase_Attenuator_controller::voltage_index_search(int ant, int power_idx ,float phase){
  while(phase > 360)
    phase -= 360.0;
  while(phase < 0)
    phase += 360.0;

  return find_matched_preset(ant, power_idx, phase);
}

int Phase_Attenuator_controller::phase_setup(int ant, int power_idx, int index){
  ant_power_setting[ant] = power_idx;
  int at_result = V.voltage_modify(ATTENUATOR[ant], V_preset[ant][power_idx][index].po_V);
  int ph_result = V.voltage_modify(PHASE[ant], V_preset[ant][power_idx][index].ph_V);

  return at_result&&ph_result;
}

int Phase_Attenuator_controller::phase_control(int ant, float power, int phase){
  if(ant_power_setting[ant]==PoffIDX) { //If this ant is offed, we do nothing
    std::cout<<"It's offed!"<<std::endl;
    return 0;
  }
  ant_power_setting[ant] = dB2idx(power);
  return phase_control(ant, phase);
}

int Phase_Attenuator_controller::phase_control(int ant, float power, float phase){
  if(ant_power_setting[ant]==PoffIDX) { //If this ant is offed, we do nothing
    std::cout<<"It's offed!"<<std::endl;
    return 0;
  }
  ant_power_setting[ant] = dB2idx(power);
  return phase_control(ant, phase);
}

int Phase_Attenuator_controller::phase_control(int ant, int phase){
  if(ant_power_setting[ant]==PoffIDX) { //If this ant is offed, we do nothing
    std::cout<<"It's offed!"<<std::endl;
    return 0;
  }
  int index = voltage_index_search(ant, ant_power_setting[ant], phase);
  return phase_setup(ant, ant_power_setting[ant], index);
}

int Phase_Attenuator_controller::phase_control(int ant, float phase){
  if(ant_power_setting[ant]==PoffIDX) { //If this ant is offed, we do nothing
    std::cout<<"It's offed!"<<std::endl;
    return 0;
  }  
  int index = voltage_index_search(ant, ant_power_setting[ant], phase);
  return phase_setup(ant, ant_power_setting[ant], index);
}

int Phase_Attenuator_controller::data_apply(void){
  return V.data_apply();
}

int Phase_Attenuator_controller::ant_off(int ant_num){
  ant_power_setting[ant_num] = PoffIDX;
  return V.voltage_modify(ATTENUATOR[ant_num], 0);
}

int Phase_Attenuator_controller::ant_on(int ant_num, float power){
  ant_power_setting[ant_num] = dB2idx(power);
  return phase_control(ant_num, power, 0);
}



Phase_Attenuator_controller::Phase_Attenuator_controller(void){
  init();
}

Phase_Attenuator_controller::Phase_Attenuator_controller(int phase){
  init();

  int result = 0;
  for(int i = 0; i<ANT_num; i++){
    result = result || phase_control(i, phase);
  }
  result = result || data_apply();

  if(result)
    std::cout<<"Error : Phase initalize error"<<std::endl;
}

Phase_Attenuator_controller::Phase_Attenuator_controller(float phase){
  init();

  int result = 0;
  for(int i = 0; i<ANT_num; i++){
    result = result || phase_control(i, phase);
  }
  result = result || data_apply();

  if(result)
    std::cout<<"Error : Phase initalize error"<<std::endl;
}

int Phase_Attenuator_controller::init(void){
  if(load_cal_data())
    std::cout<<"Error : Loading calibration data failed"<<std::endl;
  set_integer_index();

  std::fill_n(ant_power_setting, ANT_num, DEFAULT_POWER_idx);
  std::cout << ant_power_setting[0]<<std::endl;

  return 0;
}
