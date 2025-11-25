#
#  Breakout makefile
#
#   run "make" to compile Breakout
#   run "make remake" to compile everything from scratch
#   run "make clean" to clean up the object files
#

# C++ Compiler
CC     = g++
# Compiler flags
#CFLAGS = -Wall -O3
CFLAGS = -Wall -g

# SDL3 flags (attempt pkg-config first, fallback to sdl3-config, then -lSDL3)
SDL_CFLAGS := $(shell pkg-config --cflags sdl3 2>/dev/null || sdl3-config --cflags 2>/dev/null)
SDL_LDFLAGS := $(shell pkg-config --libs sdl3 2>/dev/null || sdl3-config --libs 2>/dev/null || echo -lSDL3)

# Append SDL include flags to compile flags
CFLAGS += $(SDL_CFLAGS)


default: breakout.exe
remake: clean default

clean:
	rm *.o

breakout.exe: p_engine.o main.o ptypes.o base.o
	$(CC) $(CFLAGS) -o breakout.exe p_engine.o main.o ptypes.o base.o $(SDL_LDFLAGS)

p_engine.o: p_engine.cpp p_engine.h
	$(CC) $(CFLAGS) -c p_engine.cpp -o p_engine.o

main.o: main.cpp p_engine.h ptypes.h
	$(CC) $(CFLAGS) -c main.cpp -o main.o

ptypes.o: ptypes.cpp ptypes.h p_engine.h data.h
	$(CC) $(CFLAGS) -c ptypes.cpp -o ptypes.o

base.o: base.cpp base.h
	$(CC) $(CFLAGS) -c base.cpp -o base.o
