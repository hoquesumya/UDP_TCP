CC = g++
CXX = g++

CFLAGS = -Wall -g
CXXFLAGS = -Wall -g -std=c++20 -pthread

LDFLAGS = -pthread

exe = server
objects = mrt_server.o server.o segment.o

.PHONY: default
default: $(exe)

$(exe): $(objects)
	$(CXX) $(LDFLAGS) $(objects) -o $(exe)

server.o: server.cpp mrt_server.h
	$(CXX) $(CXXFLAGS) -c server.cpp

segment.o:../config/segment.cpp ../config/segment.h
	$(CXX) $(CXXFLAGS) -c ../config/segment.cpp

mrt_server.o: mrt_server.cpp mrt_server.h ../config/segment.h
	$(CXX) $(CXXFLAGS) -c mrt_server.cpp

.PHONY: clean
clean:
	rm -f *~ a.out core server *.o $(exe) $(objects)

.PHONY: all
all: clean default