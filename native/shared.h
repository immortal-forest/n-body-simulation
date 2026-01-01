#ifndef SHARED_H
#define SHARED_H

#define MAX_SHARED_BODIES 20000

typedef struct {
  float x;
  float y;
} VisualBody;

typedef struct {
  int step;
  int count;
  VisualBody bodies[MAX_SHARED_BODIES];
} SharedData;

SharedData *create_shared_mem();

#endif // !SHARED_H
#define SHARED_H
