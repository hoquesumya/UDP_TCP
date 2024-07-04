CC  = g++
CXX = g++

CFLAGS = -Wall -g
CXXFLAGS = -Wall -g -std=c++17 -pthread

LDFLAGS = -pthread

exe = client
objects = mrt_client.o client.o


.PHONY: default
default: $(exe)
$(exe) :  mrt_client.o 
$(objects): mrt_client.h


.PHONY:   clean
clean:
	rm -f *~ a.out core client *.o $(exe) $(objects)

.PHONY: all
all: clean default





