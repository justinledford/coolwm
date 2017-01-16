CC=gcc
CFLAGS=-g -lX11

all: coolwm

coolwm: coolwm.o
	$(CC) $(CFLAGS) $^ -o $@

coolwm.o: coolwm.c
	$(CC) $(CFLAGS) -c coolwm.c

clean:
	rm -rf coolwm.o coolwm
