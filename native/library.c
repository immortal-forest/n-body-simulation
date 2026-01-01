#include "galaxy.h"
#include "physics.h"
#include <stdlib.h>

// global state
Body *bodies = NULL;
int body_count = 0;

void sim_init(int n) {
  body_count = n;
  bodies = (Body *)malloc(n * sizeof(Body));
  init_galaxy(bodies, n);
}

void sim_step() {
  if (!bodies)
    return;

#pragma omp parallel for
  for (int i = 0; i < body_count; i++) {
    bodies[i].force.x = 0;
    bodies[i].force.y = 0;
  }

#pragma omp parallel for
  for (int i = 0; i < body_count; i++) {
    for (int j = 0; j < body_count; j++) {
      if (i == j)
        continue;
      Vector f = calculate_force(&bodies[i], &bodies[j]);
      bodies[i].force.x += f.x;
      bodies[i].force.y += f.y;
    }
  }

#pragma omp parallel for
  for (int i = 0; i < body_count; i++) {
    update_vectors(&bodies[i], TIME_STEP);
  }
}

Body *sim_get_bodies() { return bodies; }

void sim_cleanup() {
  if (bodies) {
    free(bodies);
    bodies = NULL;
  }
}
