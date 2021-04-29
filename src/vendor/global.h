#pragma once
#include <pthread.h>
#include <stdio.h>

typedef struct
{
    char *readFrom;
    char *writeTo;
    unsigned in;
    unsigned out;
    pthread_t *coordinator;
    struct
    {
        char reset : 1;
        char done : 1;
    } flag;
} globals;

/**
 * Constructs a new global object
 *
 **/
globals *ctor_global ();

/**
 * Destroys global object
 *
 **/
void dtor_global (globals *g);
