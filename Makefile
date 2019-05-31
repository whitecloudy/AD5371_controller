CC = g++
CC_VERSION = -std=c++11
CFLAGS = -W -Wall -g
TARGET = $(TARGET1) $(TARGET2)
TARGET1 = beamforming
TARGET2 = voltage_control
LINK = -lpthread -pthread
OBJECTS = SPI_communicator.o Vout_controller.o Phase_Attenuator_controller.o Adaptive_beamformer.o IPC_controller.o

all : $(TARGET) 

$(TARGET1) : $(TARGET1).o $(OBJECTS)
	$(CC) $(CFLAGS) $(CC_VERSION) -o $@ $^ $(LINK)

$(TARGET2) : $(TARGET2).o $(OBJECTS)
	$(CC) $(CFLAGS) $(CC_VERSION) -o $@ $^ $(LINK)



clean : 
	rm *.o $(TARGET)
