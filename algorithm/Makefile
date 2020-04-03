##
#	Makefile for the manuscript submitted to VLDBJ'20,
#	Author: Trasier
##
CC = gcc
CXX = g++
LIBS = 
CFLAGS = --std=c++11 -W -Wall -Wno-sign-compare -O3 -s -pipe -mmmx -msse -msse2 -msse3 # -mcmodel=medium
MEM = -D WATCH_MEM

all: pruneGDP GDP
monitor.o: monitor.h monitor.cpp
	$(CXX) $(CFLAGS) -c monitor.cpp $(LIBS) -D WATCH_MEM
	
global.o: global.h global.cpp
	$(CXX) $(CFLAGS) -c global.cpp $(LIBS)

metric.o: lrucache.h labels.h metric.h metric.cpp
	$(CXX) $(CFLAGS) -c metric.cpp $(LIBS)
	
util.o: util.h util.cpp monitor.h monitor.cpp monitor.o global.h global.cpp global.o labels.h metric.h metric.cpp metric.o
	$(CXX) $(CFLAGS) -c util.cpp monitor.o $(LIBS) $(MEM)

pruneGDP: pruneGDP.cpp global.h global.cpp global.o lrucache.h labels.h metric.h metric.cpp metric.o monitor.h monitor.cpp monitor.o util.h util.cpp util.o
	$(CXX) $(CFLAGS) -o pruneGDP global.o metric.o monitor.o util.o pruneGDP.cpp $(LIBS) $(MEM)

GDP: GDP.cpp global.h global.cpp global.o lrucache.h labels.h metric.h metric.cpp metric.o monitor.h monitor.cpp monitor.o util.h util.cpp util.o
	$(CXX) $(CFLAGS) -o GDP global.o metric.o monitor.o util.o GDP.cpp $(LIBS) $(MEM)

.PHONY: clean
clean:
		-@rm *.o *.gcno *~ 2> /dev/null || true
		-@rm pruneGDP GDP 2> /dev/null || true

