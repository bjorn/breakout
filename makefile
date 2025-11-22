#
#  Breakout makefile
#
#   run "make" to compile Breakout
#   run "make remake" to compile everything from scrath
#   run "make clean" to clean up the object files
#

# C++ Compiler
CC     = gcc
# Compiler flags
CFLAGS = -Wall -O3


default: breakout.exe
remake: clean default

clean:
	del *.o

breakout.exe: p_engine.o main.o ptypes.o particle_list.o
	$(CC) $(CFLAGS) -o breakout.exe p_engine.o main.o ptypes.o particle_list.o -lalleg

p_engine.o: p_engine.cpp p_engine.h
	$(CC) $(CFLAGS) -c p_engine.cpp -o p_engine.o

main.o: main.cpp p_engine.h ptypes.h
	$(CC) $(CFLAGS) -c main.cpp -o main.o

ptypes.o: ptypes.cpp ptypes.h p_engine.h datafile.h particle_list.h
	$(CC) $(CFLAGS) -c ptypes.cpp -o ptypes.o

particle_list.o: particle_list.cpp particle_list.h
	$(CC) $(CFLAGS) -c particle_list.cpp -o particle_list.o

datafile.h: breakout.dat
	dat -h datafile.h breakout.dat
	

