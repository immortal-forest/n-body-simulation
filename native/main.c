#include <stdio.h>
#include <stdlib.h>

#include "physics.h"

#define BODY_COUNT 2
#define TIME_STEP 86400.0
#define STEPS 365

void init_system(Body bodies[]) {
  // 1. The Sun (Static center)
  bodies[0].mass = 1.989e30;
  bodies[0].position.x = 0;
  bodies[0].position.y = 0;
  bodies[0].velocity.x = 0;
  bodies[0].velocity.y = 0;
  bodies[0].force.x = 0;
  bodies[0].force.y = 0;

  // 2. The Earth (Orbiting)
  bodies[1].mass = 5.972e24;
  bodies[1].position.x = 1.496e11; // 1 AU (Distance from Sun)
  bodies[1].position.y = 0;
  bodies[1].velocity.x = 0;
  bodies[1].velocity.y = 29780; // ~30km/s orbital speed
  bodies[1].force.x = 0;
  bodies[1].force.y = 0;
}

int main(int argc, char *argv[]) {
  Body bodies[BODY_COUNT];
  init_system(bodies);

  // printf("Starting simulation for %d bodies over %d steps...\n", BODY_COUNT,
  // STEPS);
  printf("Step,BodyID,X,Y\n");

  for (int step = 0; step < STEPS; step++) {

    // reset forces
    for (int i = 0; i < BODY_COUNT; i++) {
      bodies[i].force.x = 0;
      bodies[i].force.y = 0;
    }

    for (int i = 0; i < BODY_COUNT; i++) {

      for (int j = i + 1; j < BODY_COUNT; j++) {
        Vector f = calculate_force(&bodies[i], &bodies[j]);

        // Apply to Body i
        bodies[i].force.x += f.x;
        bodies[i].force.y += f.y;

        // Newton's 3rd Law: The force on j is equal and opposite
        bodies[j].force.x -= f.x;
        bodies[j].force.y -= f.y;
      }
    }

    for (int i = 0; i < BODY_COUNT; i++) {
      update_vectors(&bodies[i], TIME_STEP);
    }

    if (step % 1 == 0) { // Log every step
      printf("%d,0,%.2f,%.2f\n", step, bodies[0].position.x,
             bodies[0].position.y);
      printf("%d,1,%.2f,%.2f\n", step, bodies[1].position.x,
             bodies[1].position.y);
    }
  }

  return EXIT_SUCCESS;
}
