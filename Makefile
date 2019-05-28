CC = g++
CC_VERSION = -std=c++11
CFLAGS = -W -Wall -g
TARGET = $(TARGET1)
TARGET1 = Reader_communicator
LINK = -lwiringPi -lpthread
OBJECTS = SPI_communicator.o GPIO_communicator.o 

all : $(TARGET)

$(TARGET1) : $(TARGET1).o $(OBJECTS)
	$(CC) $(CFLAGS) $(LINK) $(CC_VERSION) -o $@ $^

clean : 
	rm *.o $(TARGET)
