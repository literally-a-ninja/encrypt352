#ifndef H_DS_RING
#define H_DS_RING

#include <semaphore.h>
#include <stdlib.h>

typedef struct
{
    char *buf;
    unsigned char head;
    unsigned char tail;
    unsigned char capacity;
    sem_t *pushSem;
    sem_t *popSem;

} ring;

void ring_push (char c, ring *r);
char ring_pop (ring *r);
ring *ctor_ring (unsigned length);

#endif
