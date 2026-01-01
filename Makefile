# Compiler Settings
CC = gcc
NVCC = nvcc
CFLAGS = -O3 -Wall -std=c11 -lm
OMP_FLAGS = -fopenmp

# Targets
all: serial parallel cuda

# 1. Serial (Normal C)
# We compile main.c but WITHOUT the -fopenmp flag. 
# The #pragma lines will be ignored.
serial: native/main.c native/physics.c
	mkdir -p build
	$(CC) $(CFLAGS) -o build/serial native/main.c native/physics.c

# 2. Parallel (OpenMP)
# We compile main.c WITH the -fopenmp flag.
parallel: native/mainp.c native/physics.c
	mkdir -p build
	$(CC) $(CFLAGS) $(OMP_FLAGS) -o build/parallel native/main.c native/physics.c

# 3. CUDA (GPU)
# We use nvcc to compile the .cu file
cuda: native/main.cu
	mkdir -p build
	$(NVCC) -O3 -o build/cuda native/main.cu

clean:
	rm -rf build data
