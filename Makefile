# See https://youtu.be/CRlqU9XzVr4 for an explanation

CC=gcc
CFLAGS=-g -Wall

SRCS=$(wildcard src/*.c)
OBJS=$(patsubst src/%.c, obj/%.o, $(SRCS))

all: bin/main

# Producing bin/main file
bin/main: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o bin/main

# Producing .o files from .c and .h files
obj/%.o: src/%.c src/%.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf bin/* obj/*