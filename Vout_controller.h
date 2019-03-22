#ifndef __VOUT_CONTROLLER__
#define __VOUT_CONTROLLER__

#include <iostream>
#include "SPI_communicator.h"

#define MAX_offset_gain 0x3FFF
#define Vout_offset_default  0x2000
#define Vout_gain_default    0x3FFF
#define Group0_offset_default 0x1555
#define Group1_offset_default 0x1555
#define Group234_offset_default 0x1555
#define DAC_offset_switch true

#define Serial_Word_Size 3
#define MODE_MASK 0xC0
#define ADDRESS_MASK 0x3F
#define DATA_MASK0 0xFFFC

#define MAX_channel_num 40

#define ALL_vout_num -1
#define Group0_ALL_vout_num -2
#define Group1_ALL_vout_num -3
#define Group2_ALL_vout_num -4
#define Group3_ALL_vout_num -5
#define Group4_ALL_vout_num -6

#define OFFSET_FILE_name "offset_saved_file.csv" //name of csv file

enum offset_types {Group0_offset = 0, Group1_offset, Group234_offset, Vout_offset, Vout_gain};


class Vout_controller{
  private:
    SPI_communicator spi_comm;  //for communicate with arduino
    unsigned char buffer[Serial_Word_Size]= {0};  //bytes buffer to send spi data

    //Vout offset and gain value for each channel
    int Vout_offset_value[MAX_channel_num] = {0};
    int Vout_gain_value[MAX_channel_num] = {0};
    
    //Vout group offset values
    int Group0_offset_value = Group0_offset_default;
    int Group1_offset_value = Group1_offset_default;
    int Group234_offset_value = Group234_offset_default;
    
  private:
    /*
     * Funcion which is checking each bits and change buffer with combined spi data
     *
     * 0<= address_function <=0x3F
     * if mode_bits == 0, 0<=data<=0xFFFF
     * if 0< mode_bits <3, 0<=data<=0x3FFF
     *
     * otherwise it returns error
     */
    int serial_word_maker(int mode_bits, int address_function, int data);

    /*
     * Function for making Vout address 
     */
    int address_maker(int vout_num);

    /*
     * Function for sending buffer to arduino
     */
    int data_sender();

    /*
     * Read offset&gain data from csv file
     */
    int offset_data_reader();

    /*
     * Write current offset&gain data to csv file
     */
    int offset_data_writer();

  public:
    Vout_controller();
    Vout_controller(int calibrated_offset_value[], int calibrated_gain_value[]);
    ~Vout_controller();
    
    /*
     * Function which is modifing offset&gain value. And send modified value to 16wayboard
     *
     * offset_num : type of offset(Group offset, Vout offset, Vout gain)
     * vout_function : number of Vout. If it is about Group offset, number of Group
     * value : value for offset or gain
     */
    int offset_modify(offset_types offset_num, int vout_function, int value);

    /*
     * Send all offset and gain data to 16wayboard for refresh
     */
    int offset_refresh();

    /*
     * Change Vout voltage with actual voltage value
     *
     * <Available Special Vout>
     * ALL_vout_num, Group0_ALL_vout_num, Group1_ALL_vout_num, Group2_ALL_vout_num, Group3_ALL_vout_num, Group4_ALL_vout_num
     */
    int voltage_modify(int vout_num, float voltage);

    /*
     * Change Vout voltage with binary code value
     *
     *<Available Special Vout>
     * ALL_vout_num, Group0_ALL_vout_num, Group1_ALL_vout_num, Group2_ALL_vout_num, Group3_ALL_vout_num, Group4_ALL_vout_num
     */
    int voltage_modify_bin(int vout_num, int code);

    /*
     * Send LDAC signal to apply sent data
     */
    int data_apply();

    /*
     * Print every offset&gain data
     */
    int print_all_offset();
};

#endif
