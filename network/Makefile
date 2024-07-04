CC  = g++
CXX = g++

CFLAGS = -Wall -g
CXXFLAGS = -Wall -g -std=c++17 -pthread

LDFLAGS = -pthread

exe = main
objects = network.o main.o


.PHONY: default
default: $(exe)
$(exe) :  network.o 
$(objects): network.h


.PHONY:   clean
clean:
	rm -f *~ a.out core main *.o $(exe) $(objects)

.PHONY: all
all: clean default





