#include "../fkworks0/debug.h"

#include "coroutines.h"
#include "encrypt-module.c"
#include "ring.h"

#include <bits/struct_rwlock.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

//#define DEBUG_COROUTINES
#define THREAD_COUNT 5

pthread_t t [THREAD_COUNT];
ring *r_rcipher = 0, *r_rcount = 0, *r_wcount = 0, *r_write = 0;

sem_t sem_reset_lock;

void print_counts (int counts [256])
{
    char c;
    for (c = 'A'; c <= 'Z'; c++)
        printf ("%c%c:%d", (c - 'A') ? ' ' : 0, c, counts [c]);

    printf ("\n");
}

void print_input_count ()
{
    printf ("Total input count with current key is %d.\n",
            get_input_total_count ());
    print_counts (input_counts);
}

void print_output_count ()
{
    printf ("Total output count with current key is %d.\n",
            get_output_total_count ());
    print_counts (output_counts);
}

void co_coordinator_init (globals *g)
{
    ring **rings []              = {&r_rcipher, &r_rcount, &r_wcount, &r_write};
    void *(*routines [])(void *) = {&co_reader, &co_rcipher, &co_rcount,
                                    &co_wcount, &co_writer};

#ifdef DEBUG_COROUTINES
    dump (g->in);
    dump (g->out);
#endif

    unsigned i;
    for (i = 0; i < 4; i++)
    {
        char bRead = i < 2;
        *rings [i] = ctor_ring (bRead ? g->in : g->out);
    }

    for (i = 0; i < 5; i++)
    {
        pthread_create (&t [i], NULL, routines [i], g);
    }
}

pthread_t *ctor_coordinator (globals *g)
{
    pthread_t *coord = malloc (sizeof (pthread_t));
    pthread_create (coord, NULL, &co_coordinator, (void *)g);

    return coord;
}

void dtor_coordinator (pthread_t *coordinator)
{
    //    debug ("dtor_coordinator() - awaiting termination...");

    pthread_join (*coordinator, NULL);

    //    debug ("dtor_coordinator() - calling dtor_ring() on all rings.");

    // Cleanup heap
    ring *rings [] = {r_rcipher, r_rcount, r_wcount, r_write};
    unsigned i;
    for (i = 0; i < 4; i++)
        dtor_ring (rings [i]);

    //    debug ("dtor_coordinator() - destroying thread");

    free (coordinator);

    //    debug ("dtor_coordinator() - done!");
}

#define THREAD_WRITE_COUNT 3
#define THREAD_WRITE_FILE 4
void *co_coordinator (void *a)
{
#ifdef DEBUG_COROUTINES
    debug ("co_coordinator(void *a) :: Invoked");
#endif

    globals *g = a;
    co_coordinator_init (g);

    pthread_setcancelstate (PTHREAD_CANCEL_ENABLE, NULL);

    // Await writer and counter
    pthread_join (t [THREAD_WRITE_COUNT], NULL);
    pthread_join (t [THREAD_WRITE_FILE], NULL);

    print_input_count ();
    print_output_count ();

#ifdef DEBUG_COROUTINES
    debug ("co_coordinator(void *a) :: End-of-thread");
#endif

    pthread_exit (0);
}

void *co_reader (void *a)
{
#ifdef DEBUG_COROUTINES
    debug ("co_reader(void *a) :: Invoked");
#endif

    globals *g = a;
    char c;
    do
    {
        c = read_input ();

        if (g->flag.reset)
            c = EOF;

        ring_push (&c, r_rcount);
        ring_push (&c, r_rcipher);
    } while (EOF != c);

#ifdef DEBUG_COROUTINES
    debug ("co_reader(void *a) :: End-of-thread");
#endif

    pthread_exit (0);
}

void *co_rcount (void *a)
{
#ifdef DEBUG_COROUTINES
    debug ("co_rcount(void *a) :: Invoked");
#endif
    char c;
    globals *g = a;
    while ((c = ring_pop (r_rcount)) != EOF)
    {
        if (g->flag.reset)
            break;

        count_input (c);
    }

#ifdef DEBUG_COROUTINES
    debug ("co_rcount(void *a) :: End-of-thread");
#endif
    pthread_exit (0);
}

void *co_rcipher (void *a)
{
#ifdef DEBUG_COROUTINES
    debug ("co_rcipher(void *a) :: Invoked");
#endif
    char c, e;
    globals *g = a;
    do
    {
        c = ring_pop (r_rcipher);
        e = (c == EOF) ? c : caesar_encrypt (c);

        ring_push (&e, r_write);
        ring_push (&e, r_wcount);
    } while (c != EOF);

#ifdef DEBUG_COROUTINES
    debug ("co_rcipher(void *a) :: End-of-thread");
#endif

    pthread_exit (0);
}

void *co_wcount (void *a)
{
#ifdef DEBUG_COROUTINES
    debug ("co_wcount(void *a) :: Invoked");
#endif

    char c;
    globals *g = a;
    while ((c = ring_pop (r_wcount)) != EOF)
    {
        count_output (c);
    };

#ifdef DEBUG_COROUTINES
    debug ("co_wcount(void *a) :: End-of-thread");
#endif

    pthread_exit (0);
}

void *co_writer (void *a)
{
#ifdef DEBUG_COROUTINES
    debug ("co_writer(void *a) :: Invoked");
#endif

    char *c    = malloc (sizeof (char));
    globals *g = a;
    for (*c = ring_pop (r_write); *c != EOF; *c = ring_pop (r_write))
    {
        write_output (*c);
    }

    free (c);

    if (!g->flag.reset)
    {
        g->flag.done = 1;
        printf ("\nEnd of file reached.\n");
    }

#ifdef DEBUG_COROUTINES
    debug ("co_writer(void *a) :: End-of-thread");
#endif

    pthread_exit (0);
}
