CXXFLAGS = $(CC_VERSION) $(CFLAGS) $(LINK)
CC_VERSION = -std=c++11
CFLAGS = -W -Wall -g
TARGET = $(TARGET1) $(TARGET2) $(TARGET3)
TARGET1 = beamforming
TARGET2 = voltage_control
LINK = -lpthread -pthread -larmadillo
OBJECTS = SPI_communicator.o Vout_controller.o Phase_Attenuator_controller.o Beamformer.o IPC_controller.o Adaptive_Beamtrainer.o

all : $(TARGET) 

$(TARGET1) : $(TARGET1).o $(OBJECTS)
		$(CXX) -o $@ $^ $(LINK)

$(TARGET2) : $(TARGET2).o $(OBJECTS)
		$(CXX) -o $@ $^ $(LINK)


clean : 
		rm *.o $(TARGET)

