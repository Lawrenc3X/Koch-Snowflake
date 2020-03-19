CC = gcc
CXX = g++
CFLAGS = -g -lncurses
CXXFLAGS = -g -lncurses

Koch: Koch.cpp
	$(CXX) $(CXXFLAGS) -o Koch Koch.cpp

all: Koch.cpp 
	$(CXX) $(CXXFLAGS) -o Koch Koch.cpp

clean:
	rm -r Koch Koch.dSYM
