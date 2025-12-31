#include "physics.h"
#include <math.h>

real calculate_distance(Vector *v1, Vector *v2) {
  real x = v2->x - v1->x;
  real y = v2->y - v1->y;
  return sqrt(x * x + y * y + E);
}

Vector calculate_force(Body *b1, Body *b2) {
  real mass_multiple = G * b1->mass * b2->mass;
  real distance = calculate_distance(&b1->position, &b2->position);
  real dist_cb = distance * distance * distance;
  real dx = b2->position.x - b1->position.x;
  real dy = b2->position.y - b1->position.y;

  Vector force;
  force.x = (mass_multiple * dx) / dist_cb;
  force.y = (mass_multiple * dy) / dist_cb;
  return force;
}

void update_vectors(Body *b, real dt) {
  Vector f = b->force;
  Vector v = b->velocity;

  real ax, ay;

  ax = f.x / b->mass;
  ay = f.y / b->mass;

  Vector vel;
  vel.x = v.x + (ax * dt);
  vel.y = v.y + (ay * dt);

  Vector pos;
  pos.x = b->position.x + (vel.x * dt);
  pos.y = b->position.y + (vel.y * dt);

  b->velocity = vel;
  b->position = pos;
}
