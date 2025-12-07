CC = gcc
CFLAGS = -O3 -std=c99 -fPIC

all: librge256ctr.so rge256ctr

# Build shared library for Python wrapper
librge256ctr.so: rge256ctr.c rge256ctr.h
	$(CC) $(CFLAGS) -shared -o ../rge256/librge256ctr.so rge256ctr.c

# Build standalone executable
rge256ctr: rge256ctr.c rge256ctr.h
	$(CC) -O3 -std=c99 rge256ctr.c -o rge256ctr

clean:
	rm -f rge256ctr
	rm -f ../rge256/librge256ctr.so
