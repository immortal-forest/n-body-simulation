#include "types.h"
#include <cstdio>
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
    double dist = sqrt(dx * dx + dy * dy + 1e9); // Softening
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

double rand_01() { return (double)rand() / (double)RAND_MAX; }

void init_random(Body *bodies, int n) {
  srand(42);

  // Scale factors
  double galaxy_radius = 4.0e11; // ~2.5 AU
  double core_mass = 1e32;       // Supermassive center

  for (int i = 0; i < n; i++) {
    // Body 0 is the Black Hole
    if (i == 0) {
      bodies[i].mass = core_mass;
      bodies[i].position.x = 0;
      bodies[i].position.y = 0;
      bodies[i].velocity.x = 0;
      bodies[i].velocity.y = 0;
      bodies[i].force.x = 0;
      bodies[i].force.y = 0;
      continue;
    }

    double angle = rand_01() * 2.0 * 3.14159;
    double dist = galaxy_radius *
                  (0.2 + 0.8 * rand_01()); // Avoid being TOO close to center

    bodies[i].position.x = dist * cos(angle);
    bodies[i].position.y = dist * sin(angle);
    bodies[i].mass = 1e24 + (rand_01() * 1e24); // Random small mass

    double velocity = sqrt(6.6743e-11 * core_mass / dist);

    bodies[i].velocity.x = -sin(angle) * velocity;
    bodies[i].velocity.y = cos(angle) * velocity;

    // Reset forces
    bodies[i].force.x = 0;
    bodies[i].force.y = 0;
  }
}

int main(int argc, char *argv[]) {
  int n = (argc > 1) ? atoi(argv[1]) : 1000;
  int steps = (argc > 2) ? atoi(argv[2]) : 100;

  // printf("GPU CUDA: Running %d bodies for %d steps...\n", n, steps);
  printf("Step,BodyID,X,Y\n");

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
    if (s % 1 == 0) {
      cudaMemcpy(h_bodies, d_bodies, size, cudaMemcpyDeviceToHost);

      for (int i = 0; i < n; i++) {
        printf("%d,%d,%.2f,%.2f\n", s, i, h_bodies[i].position.x,
               h_bodies[i].position.y);
      }
    }
  }

  cudaFree(d_bodies);
  free(h_bodies);
  return 0;
}
