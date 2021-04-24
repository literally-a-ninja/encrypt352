#ifndef H_DS_RING
#define H_DS_RING

#include <semaphore.h>
typedef struct
{
    char *buf;
    unsigned capacity;
    unsigned head, tail;
    sem_t *pushSem, *popSem;
} ring;

void ring_push (char c, ring *r);
char ring_pop (ring *r);
void ring_reset (ring *r);
void dtor_ring (ring *r);
ring *ctor_ring (unsigned length);

#endif // H_DS_RING

