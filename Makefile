CC = g++
CC_VERSION = -std=c++11
CFLAGS = -W -Wall -g
TARGET = $(TARGET1)
TARGET1 = Reader_communicator
LINK = -lwiringPi -lpthread -lbcm2835
OBJECTS = SPI_communicator.o GPIO_communicator.o 

all : $(TARGET)

$(TARGET1) : $(TARGET1).o $(OBJECTS)
	$(CC) $(CFLAGS) $(CC_VERSION) -o $@ $^ $(LINK) 

clean : 
	rm *.o $(TARGET)
