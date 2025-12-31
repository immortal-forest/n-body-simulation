#ifndef PHYSICS_H
#define PHYSICS_H

#include "types.h"

static const real G = 6.6743e-11;
static const real E = 1e-2;

real calculate_distance(Vector *, Vector *);
Vector calculate_force(Body *, Body *);
void update_vectors(Body *, real);

#endif /* ifndef PHYSICS_H */
