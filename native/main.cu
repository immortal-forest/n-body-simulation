#include "types.h"
#include <cuda_runtime.h>
#include <stdio.h>
#include <stdlib.h>

#define BLOCK_SIZE 256 // Threads per block

// Runs on GPU. Each thread handles ONE body (index i).
__global__ void compute_forces(Body *bodies, int n, double G) {
  int i = blockIdx.x * blockDim.x + threadIdx.x;
  if (i >= n)
    return;

  Body my_body = bodies[i];
  double fx = 0.0;
  double fy = 0.0;

  // The GPU Loop: Check every other body
  for (int j = 0; j < n; j++) {
    if (i == j)
      continue;

    Body other = bodies[j];
    double dx = other.position.x - my_body.position.x;
    double dy = other.position.y - my_body.position.y;
    double dist = sqrt(dx * dx + dy * dy + 1e-2); // Softening
    double f = (G * my_body.mass * other.mass) / (dist * dist * dist);

    fx += f * dx;
    fy += f * dy;
  }

  bodies[i].force.x = fx;
  bodies[i].force.y = fy;
}

__global__ void update_physics(Body *bodies, int n, double dt) {
  int i = blockIdx.x * blockDim.x + threadIdx.x;
  if (i >= n)
    return;

  double ax = bodies[i].force.x / bodies[i].mass;
  double ay = bodies[i].force.y / bodies[i].mass;

  bodies[i].velocity.x += ax * dt;
  bodies[i].velocity.y += ay * dt;

  bodies[i].position.x += bodies[i].velocity.x * dt;
  bodies[i].position.y += bodies[i].velocity.y * dt;

  bodies[i].force.x = 0;
  bodies[i].force.y = 0;
}

void init_random(Body *bodies, int n) {
  for (int i = 0; i < n; i++) {
    bodies[i].mass = (i == 0) ? 1e32 : 1e24;
    bodies[i].position.x = (i == 0) ? 0 : (rand() % 1000 - 500) * 1e9;
    bodies[i].position.y = (i == 0) ? 0 : (rand() % 1000 - 500) * 1e9;
    bodies[i].velocity.x = 0;
    bodies[i].velocity.y = 0;
  }
}

int main(int argc, char *argv[]) {
  int n = (argc > 1) ? atoi(argv[1]) : 1000;
  int steps = (argc > 2) ? atoi(argv[2]) : 100;

  printf("GPU CUDA: Running %d bodies for %d steps...\n", n, steps);

  size_t size = n * sizeof(Body);
  Body *h_bodies = (Body *)malloc(size);
  init_random(h_bodies, n);

  Body *d_bodies;
  cudaMalloc(&d_bodies, size);

  cudaMemcpy(d_bodies, h_bodies, size, cudaMemcpyHostToDevice);

  int blocks = (n + BLOCK_SIZE - 1) / BLOCK_SIZE;

  for (int s = 0; s < steps; s++) {
    compute_forces<<<blocks, BLOCK_SIZE>>>(d_bodies, n, 6.6743e-11);
    cudaDeviceSynchronize(); // Wait for calc to finish

    update_physics<<<blocks, BLOCK_SIZE>>>(d_bodies, n, 86400.0);
    cudaDeviceSynchronize(); // Wait for update to finish
  }

  // cudaMemcpy(h_bodies, d_bodies, size, cudaMemcpyDeviceToHost);

  printf("GPU Done.\n");
  cudaFree(d_bodies);
  free(h_bodies);
  return 0;
}
