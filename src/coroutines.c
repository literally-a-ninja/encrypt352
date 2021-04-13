#include "coroutines.h"
#include "common.h"
#include "ring.h"
#include "vendor/encrypt-module.h"

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>

#define DEBUG_COROUTINES

ring *r_rcipher;
ring *r_rcount;
ring *r_wcount;
ring *r_write;

void *co_coordinator (void *a)
{
#ifdef DEBUG_COROUTINES
    printf ("D: co_coordinator(void *a) :: Invoked\n");
#endif

    globals *g = a;

    // TODO: Make this less of an eye sore.
    r_rcipher = ctor_ring (g->in);
    r_rcount  = ctor_ring (g->in);
    r_wcount  = ctor_ring (g->out);
    r_write   = ctor_ring (g->out);

    pthread_t t [5];
    pthread_create (&t [0], NULL, &co_reader, a);
    pthread_create (&t [1], NULL, &co_rcipher, a);
    pthread_create (&t [2], NULL, &co_rcount, a);
    pthread_create (&t [3], NULL, &co_wcount, a);
    pthread_create (&t [4], NULL, &co_writer, a);

    pthread_join (t [3], NULL);
    pthread_join (t [4], NULL);

    //   dtor_ring (r_rcount);
    //   dtor_ring (r_rcipher);
    //   dtor_ring (r_wcount);
    //   dtor_ring (r_write);

#ifdef DEBUG_COROUTINES
    printf ("D: co_coordinator(void *a) :: End-of-thread\n");
#endif

    pthread_exit (0);
}

void *co_reader (void *a)
{
#ifdef DEBUG_COROUTINES
    printf ("D: co_reader(void *a) :: Invoked\n");
#endif

    char c;
    do
    {
        c = read_input ();
        ring_push (c, r_rcount);
        ring_push (c, r_rcipher);
    } while (EOF != c);

    exit (0);

#ifdef DEBUG_COROUTINES
    printf ("D: co_reader(void *a) :: End-of-thread\n");
#endif
    pthread_exit (0);
}

void *co_rcount (void *a)
{
#ifdef DEBUG_COROUTINES
    printf ("D: co_rcount(void *a) :: Invoked\n");
#endif
    char c;
    globals *g = a;
    while ((c = ring_pop (r_rcount)) != EOF)
    {
        count_input (c);
    };

#ifdef DEBUG_COROUTINES
    printf ("D: co_rcount(void *a) :: End-of-thread\n");
#endif
    pthread_exit (0);
}

void *co_rcipher (void *a)
{
#ifdef DEBUG_COROUTINES
    printf ("D: co_rcipher(void *a) :: Invoked\n");
#endif
    char c, e;
    globals *g = a;
    while ((c = ring_pop (r_rcipher)) != EOF)
    {
        e = caesar_encrypt (c);
        ring_push (e, r_write);
        ring_push (e, r_wcount);
    }

#ifdef DEBUG_COROUTINES
    printf ("D: co_rcipher(void *a) :: End-of-thread\n");
#endif
    pthread_exit (0);
}

void *co_wcount (void *a)
{
#ifdef DEBUG_COROUTINES
    printf ("D: co_wcount(void *a) :: Invoked\n");
#endif

    char c;
    globals *g = a;
    while ((c = ring_pop (r_wcount)) != EOF)
    {
        count_output (c);
    };

#ifdef DEBUG_COROUTINES
    printf ("D: co_wcount(void *a) :: End-of-thread\n");
#endif

    pthread_exit (0);
}

void *co_writer (void *a)
{
#ifdef DEBUG_COROUTINES
    printf ("D: co_writer(void *a) :: Invoked\n");
#endif

    char c;
    globals *g = a;
    while ((c = ring_pop (r_write)) != EOF)
    {
        write_output (c);
    };

#ifdef DEBUG_COROUTINES
    printf ("D: co_writer(void *a) :: End-of-thread\n");
#endif

    pthread_exit (0);
}
