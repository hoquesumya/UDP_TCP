CC  = g++
CXX = g++

CFLAGS = -Wall -g
CXXFLAGS = -Wall -g -std=c++17 -pthread

LDFLAGS = -pthread

exe = server
objects = mrt_server.o server.o


.PHONY: default
default: $(exe)
$(exe) :  mrt_server.o 
$(objects): mrt_server.h


.PHONY:   clean
clean:
	rm -f *~ a.out core server *.o $(exe) $(objects)

.PHONY: all
all: clean default





