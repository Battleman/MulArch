
CC = gcc
CFLAGS = -g -std=gnu99 -O3 -fopenmp -Wall

all: assignment2

assignment2: assignment2.c utility.h algorithm.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f assignment2
