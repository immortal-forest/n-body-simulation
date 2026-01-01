#ifndef GALAXY_H
#define GALAXY_H
#define DEFAULT_BODIES 1000
#define DEFAULT_STEPS 5000
#define TIME_STEP 86400.0 // 1 Day

#include "types.h"

real random_real(real, real);
void init_galaxy(Body *, int);

#endif // !GALAXY_H
