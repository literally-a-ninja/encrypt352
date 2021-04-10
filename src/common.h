#ifndef H_COMMON
#define H_COMMON

#include <stdlib.h>

#include "coroutines.h"
#include "vendor/encrypt-module.h"

typedef struct
{
    char *r, *w, *rBuf, *wBuf;
} globals;

/**
 * Allocates global space
 **/
globals *ctor_globals ();

/**
 * Deallocates global space
 **/
void dtor_globals (globals *g);

#endif
