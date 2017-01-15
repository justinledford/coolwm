CC=gcc
CFLAGS=-lX11

all: coolwm

coolwm: coolwm.o
	$(CC) $(CFLAGS) $^ -o $@

coolwm.o:
	$(CC) $(CFLAGS) -c coolwm.c

clean:
	rm -rf coolwm.o coolwm
