#ifndef TYPES_H
#define TYPES_H

typedef double real;

typedef struct {
  real x, y;
} Vector;

typedef struct {
  real mass;
  Vector position;
  Vector velocity;
  Vector force;
} Body;

#endif /* ifndef TYPES_H */
