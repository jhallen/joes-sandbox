#ifndef _Irandom
#define _Irandom 1

#include "config.h"

long random();
void srandom();
void *initstate();
void *setstate();

#endif
