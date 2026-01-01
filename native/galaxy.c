#include "galaxy.h"
#include <math.h>
#include <stdlib.h>

real random_real(real min, real max) {
  return min + (rand() / (real)RAND_MAX) * (max - min);
}

void init_galaxy(Body bodies[], int count) {
  // seed
  // srand(42);

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
