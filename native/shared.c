#include "shared.h"
#include "galaxy.h"
#include "physics.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

SharedData *create_shared_mem() {
  // create a shared memory with 0600 perms, rw
  int fd = shm_open("/shared_galaxy", O_CREAT | O_RDWR, 0600);

  // truncate to SharedData size
  ftruncate(fd, sizeof(SharedData));

  // pointer to the shared memory
  void *mem =
      mmap(NULL, sizeof(SharedData), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  return (SharedData *)mem;
}

int main(int argc, char *argv[]) {
  int body_count = DEFAULT_BODIES;
  int steps = DEFAULT_STEPS;

  if (argc > 1) {
    body_count = atoi(argv[1]);
  }
  if (argc > 2) {
    steps = atoi(argv[2]);
  }

  // Safety checks
  if (body_count <= 0 || steps <= 0) {
    fprintf(stderr, "Error: Bodies and Steps must be > 0\n");
    return 1;
  }

  Body *bodies = malloc(body_count * sizeof(Body));
  if (bodies == NULL) {
    fprintf(stderr, "Error: Failed to allocate memory. Too many bodies?\n");
    return 1;
  }

  init_galaxy(bodies, body_count);
  SharedData *mem = create_shared_mem();
  mem->count = body_count; // Tell Python how many to read

  for (int step = 0; step < steps; step++) {

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
        // Safety check: Don't overflow if user requested 50,000 bodies
        if (i >= MAX_SHARED_BODIES)
          break;

        mem->bodies[i].x = (float)bodies[i].position.x;
        mem->bodies[i].y = (float)bodies[i].position.y;
      }
    }
    mem->step = step;
    usleep(1000);
  }

  munmap(mem, sizeof(SharedData));
  shm_unlink("/shared_galaxy");
  free(bodies);
  return EXIT_SUCCESS;
}
