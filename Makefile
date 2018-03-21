OBJS=$(wildcard src/*.c)

CC=clang

CFLAGS=-Wall -g

LDFLAGS=

TARGET=check

all : $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET) $(LDFLAGS)