CC = g++
CC_VERSION = -std=c++11
CFLAGS = -W -Wall -g
TARGET = $(TARGET1) $(TARGET2) $(TARGET3)
TARGET1 = beamforming
TARGET2 = voltage_control
TARGET3 = board_calibration
LINK = -lpthread -pthread
OBJECTS = SPI_communicator.o Vout_controller.o Phase_Attenuator_controller.o Beamformer.o IPC_controller.o SIC_controller.o

all : $(TARGET) 

$(TARGET1) : $(TARGET1).o $(OBJECTS)
	$(CC) $(CFLAGS) $(CC_VERSION) -o $@ $^ $(LINK)

$(TARGET2) : $(TARGET2).o $(OBJECTS)
	$(CC) $(CFLAGS) $(CC_VERSION) -o $@ $^ $(LINK)

$(TARGET3) : $(TARGET3).o $(OBJECTS)
	$(CC) $(CFLAGS) $(CC_VERSION) -o $@ $^ $(LINK)



clean : 
	rm *.o $(TARGET)
