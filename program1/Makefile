# Example makefile for CPE464 Trace program
#
# Just links in pcap library

CC = gcc
LIBS = -lpcap
CFLAGS = -g -Wall -pedantic -std=gnu99 -I.

INC_DIR = inc
SRC_DIR = src
OBJ_DIR = obj
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:.c=.o)


all:  trace

trace: $(OBJS)
	$(CC) $(OBJS) -g -o trace  $(LIBS)

%.o: %.c $(INC_DIR)/%.h
	$(CC) $(CFLAGS) -c $< -o $@

debug: $(OBJS)
	$(CC) -DDEBUG $(OBJS) -o trace  $(LIBS)

clean:
	rm -f trace
