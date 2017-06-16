CC=g++
CFLAGS= -std=c++0x -I.

assign5: functions.cpp main.cpp
	$(CC) -o assign5 functions.cpp main.cpp $(CFLAGS).

