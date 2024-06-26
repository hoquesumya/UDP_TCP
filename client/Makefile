CC  = g++
CXX = g++

CFLAGS = -Wall -g
CXXFLAGS = -Wall -g -std=c++17 -pthread

LDFLAGS = -pthread
exe = client

.PHONY: default
default: $(exe)

client : client.o

.PHONY:   clean
clean:
	rm -f *~ a.out client *.o $(exe)

.PHONY: all
all: clean default





