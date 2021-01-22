CC=gcc
CPP = g++

all:
	$(CPP) tinyfuzz.cpp -o tinyfuzz
	$(CC) examples/target.c -o target

clean:
	rm tinyfuzz target
