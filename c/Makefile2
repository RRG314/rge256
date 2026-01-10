CC = gcc
CFLAGS = -O3 -std=c99 -fPIC

all: librge256drbg.so rge256drbg

# Build shared library for Python wrapper (core only)
librge256drbg.so: rge256_drbg.c rge256_drbg.h
	$(CC) $(CFLAGS) -shared -o ../rge256/librge256drbg.so rge256_drbg.c

# Build standalone executable (core + CLI)
rge256drbg: rge256_drbg.c rge256_drbg_cli.c rge256_drbg.h
	$(CC) -O3 -std=c99 rge256_drbg.c rge256_drbg_cli.c -o rge256drbg

clean:
	rm -f rge256drbg
	rm -f ../rge256/librge256drbg.so
