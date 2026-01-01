#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "physics.h"

#define TIME_STEP 86400.0
#define STEPS 365

real random_real(real min, real max) {
  return min + (rand() / (real)RAND_MAX) * (max - min);
}

void init_system(Body bodies[], int count) {
  // seed
  srand(42);

  for (int i = 0; i < count; i++) {
    if (i == 0) {
      bodies[i].mass = 1e32; // Supermassive black hole / Sun
      bodies[i].position.x = 0;
      bodies[i].position.y = 0;
      bodies[i].velocity.x = 0;
      bodies[i].velocity.y = 0;
    } else {
      bodies[i].mass = random_real(1e20, 1e25); // Smaller stars

      // Position: Random spread in a 5 AU box
      // 1 AU = ~1.5e11 meters
      real spread = 1.5e11 * 5.0;
      bodies[i].position.x = random_real(-spread, spread);
      bodies[i].position.y = random_real(-spread, spread);

      // Velocity: Give them a kick so they orbit instead of falling straight in
      // (Simulating a rotating galaxy disc roughly)
      real dist = sqrt(bodies[i].position.x * bodies[i].position.x +
                       bodies[i].position.y * bodies[i].position.y);

      // Orbital velocity, v = sqrt(GM/r) roughly
      real orbital_speed = sqrt(6.6743e-11 * 1e32 / dist);

      // Tangent direction (simplified circular orbit logic)
      // If pos is (x, y), tangent is (-y, x)
      bodies[i].velocity.x = (-bodies[i].position.y / dist) * orbital_speed;
      bodies[i].velocity.y = (bodies[i].position.x / dist) * orbital_speed;
    }

    bodies[i].force.x = 0;
    bodies[i].force.y = 0;
  }
}
int main(int argc, char *argv[]) {
  int body_count = 100; // default

  if (argv[1] != NULL) {
    body_count = atoi(argv[1]);
  }

  Body *bodies = malloc(body_count * sizeof(Body));
  if (bodies == NULL) {
    fprintf(stderr, "Error: Failed to allocate memory. Too many bodies?\n");
    return 1;
  }

  init_system(bodies, body_count);

  printf("Step,BodyID,X,Y\n");

  for (int step = 0; step < STEPS; step++) {

    // reset forces
    for (int i = 0; i < body_count; i++) {
      bodies[i].force.x = 0;
      bodies[i].force.y = 0;
    }

    for (int i = 0; i < body_count; i++) {

      for (int j = i + 1; j < body_count; j++) {
        Vector f = calculate_force(&bodies[i], &bodies[j]);

        // Apply to Body i
        bodies[i].force.x += f.x;
        bodies[i].force.y += f.y;

        // Newton's 3rd Law: The force on j is equal and opposite
        bodies[j].force.x -= f.x;
        bodies[j].force.y -= f.y;
      }
    }

    for (int i = 0; i < body_count; i++) {
      update_vectors(&bodies[i], TIME_STEP);
    }

    if (step % 1 == 0) { // Log every step
      for (int i = 0; i < body_count; i++) {
        printf("%d,%d,%.2f,%.2f\n", step, i, bodies[i].position.x,
               bodies[i].position.y);
      }
    }
  }

  free(bodies);
  return EXIT_SUCCESS;
}
