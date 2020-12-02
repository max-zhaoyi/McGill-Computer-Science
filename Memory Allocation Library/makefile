CC=gcc
CFLAGS=-fsanitize=signed-integer-overflow -fsanitize=undefined -g -std=gnu99 -O2 -Wall -Wextra -Wno-sign-compare -Wno-unused-parameter -Wno-unused-variable -Wshadow

all: a3_test.c sma.c sma.h
	$(CC) -g a3_test.c sma.c