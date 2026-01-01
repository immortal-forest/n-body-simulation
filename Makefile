# Compiler Settings
CC = gcc
CFLAGS = -O3 -Wall -std=c11
OMP_FLAGS = -fopenmp
LIBS = -lm

run: native/main.c native/galaxy.c native/physics.c
	mkdir -p build
	$(CC) $(CFLAGS) $(OMP_FLAGS) -o build/galaxy native/main.c native/galaxy.c native/physics.c $(LIBS)


# shared memory executable
shared_mem: native/shared.c native/galaxy.c native/physics.c
	mkdir -p build
	$(CC) $(CFLAGS) $(OMP_FLAGS) -o build/shared_mem native/shared.c native/galaxy.c native/physics.c $(LIBS) -lrt
	
# shared library
library: native/library.c native/galaxy.c native/physics.c
	mkdir -p build
	$(CC) $(CFLAGS) $(OMP_FLAGS) -fPIC -shared -o build/libgalaxy.so native/library.c native/galaxy.c native/physics.c $(LIBS)

clean:
	rm -rf build
