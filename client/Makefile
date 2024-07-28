CC = g++
CXX = g++

CFLAGS = -Wall -g
CXXFLAGS = -Wall -g -std=c++20 -pthread

LDFLAGS = -pthread

exe = client
objects = mrt_client.o client.o segment.o

.PHONY: default
default: $(exe)

$(exe): $(objects)
	$(CXX) $(LDFLAGS) $(objects) -o $(exe)

client.o: client.cpp mrt_client.h
	$(CXX) $(CXXFLAGS) -c client.cpp

segment.o:../config/segment.cpp ../config/segment.h
	$(CXX) $(CXXFLAGS) -c ../config/segment.cpp

mrt_client.o: mrt_client.cpp mrt_client.h ../config/segment.h
	$(CXX) $(CXXFLAGS) -c mrt_client.cpp

.PHONY: clean
clean:
	rm -f *~ a.out core client *.o $(exe) $(objects)

.PHONY: all
all: clean default