#include "ring.h"
#include <semaphore.h>
#include <stdlib.h>

char ring_pop (ring *r)
{
    // Spinlock while there's nothing in our ring.
    sem_wait (r->popSem);
    sem_post (r->pushSem);

    char c;
    c       = r->buf [r->tail];
    r->tail = r->tail + 1 >= r->capacity ? 0 : r->tail + 1;

    return c;
}

void ring_reset (ring *r)
{
    r->head = 0;
    r->tail = 0;

    sem_close (r->popSem);
    sem_close (r->pushSem);
    sem_init (r->popSem, 1, 0);
    sem_init (r->pushSem, 1, r->capacity);
}

void ring_push (char c, ring *r)
{
    // Spinlock while there's no availability.
    sem_wait (r->pushSem);
    sem_post (r->popSem);

    r->buf [r->head] = c;
    r->head          = r->head + 1 >= r->capacity ? 0 : r->head + 1;
}

void dtor_ring (ring *r)
{
    free (r->buf);
    free (r->pushSem);
    free (r->popSem);
    free (r);
}

ring *ctor_ring (unsigned length)
{

    ring *r    = malloc (sizeof (ring));
    r->popSem  = malloc (sizeof (sem_t));
    r->pushSem = malloc (sizeof (sem_t));
    r->buf     = malloc (sizeof (char) * length);

    r->capacity = length;
    ring_reset (r);

    return r;
}

