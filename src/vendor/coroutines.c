#include "../fkworks0/debug.h"

#include "coroutines.h"
#include "encrypt-module.c"
#include "encrypt-module.h"
#include "global.h"
#include "ring.h"

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// #define DEBUG_COROUTINES

ring *r_rcipher = 0;
ring *r_rcount  = 0;
ring *r_wcount  = 0;
ring *r_write   = 0;

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

void reset_requested ()
{
    printf ("Reset requested.\n");
    print_input_count ();
    print_output_count ();
    // TODO: Stop threads
    // TODO: Clear file buffers
    // TODO: Clear buffers
    // TODO: Resume threads
}

void reset_finished ()
{
    printf ("Reset finished.\n");
}

void *random_reset ()
{
#ifdef DEBUG_COROUTINES
    debug ("random_reset() :: Invoked\n");
#endif
    while (1)
    {
#ifdef DEBUG_COROUTINES
        sleep (rand () % 11 + 1);
#else
        sleep (rand () % 11 + 5);
#endif
        reset_requested ();
        key = rand () % 26;
        clear_counts ();
        reset_finished ();
    }
#ifdef DEBUG_COROUTINES
    debug ("random_reset() :: End-of-thread\n");
#endif
}

void co_coordinator_init (globals *g, pthread_t t [])
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

        if (*rings [i])
        {
            dtor_ring (*rings [i]);
        }

        *rings [i] = ctor_ring (bRead ? g->in : g->out);
    }

    for (i = 0; i < 5; i++)
    {
        pthread_create (&t [i], NULL, routines [i], g);
    }
}

pthread_t *ctor_coordinator (globals *g)
{
    init (g->readFrom, g->writeTo);
    pthread_t *coord = malloc (sizeof (pthread_t));
    pthread_create (coord, NULL, &co_coordinator, (void *)g);

    return coord;
}

void co_coordinator_reset (pthread_t t [])
{
    debug ("dtor_coordinator_reset() - Awaiting for worker threads to stop...");
    pthread_t *tr;

    // Send our cancel request.
    for (tr = t; tr; tr++)
        pthread_cancel (*tr);

    // Wait until the thread exits.
    for (tr = t; tr; tr++)
        pthread_join (*tr, NULL);

    debug ("dtor_coordinator_reset() - Threads are dead; let's move on.");

    // Cleanup heap
    ring *rg, *rings [] = {r_rcipher, r_rcount, r_wcount, r_write};
    for (rg = *rings; rg; rg++)
        dtor_ring (rg);

    debug ("dtor_coordinator_reset() - dtor_ring() called on all rings.");
}

#define THREAD_WRITE_COUNT 3
#define THREAD_WRITE_FILE 4
void *co_coordinator (void *a)
{
#ifdef DEBUG_COROUTINES
    debug ("co_coordinator(void *a) :: Invoked\n");
#endif

    globals *g = a;
    pthread_t t [5];
    co_coordinator_init (g, t);

    pthread_setcancelstate (PTHREAD_CANCEL_ENABLE, NULL);

    // Await writer and counter
    pthread_join (t [THREAD_WRITE_COUNT], NULL);
    pthread_join (t [THREAD_WRITE_FILE], NULL);

    // Dump our counts
    print_input_count ();
    print_output_count ();

#ifdef DEBUG_COROUTINES
    debug ("co_coordinator(void *a) :: End-of-thread\n");
#endif

    pthread_exit (0);
}

void *co_reader (void *a)
{
#ifdef DEBUG_COROUTINES
    debug ("co_reader(void *a) :: Invoked\n");
#endif

    char c;
    do
    {
        c = read_input ();
        ring_push (c, r_rcount);
        ring_push (c, r_rcipher);
    } while (EOF != c);

#ifdef DEBUG_COROUTINES
    debug ("co_reader(void *a) :: End-of-thread\n");
#endif

    pthread_exit (0);
}

void *co_rcount (void *a)
{
#ifdef DEBUG_COROUTINES
    debug ("co_rcount(void *a) :: Invoked\n");
#endif
    char c;
    globals *g = a;
    while ((c = ring_pop (r_rcount)) != EOF)
    {
        count_input (c);
    };

#ifdef DEBUG_COROUTINES
    debug ("co_rcount(void *a) :: End-of-thread\n");
#endif
    pthread_exit (0);
}

void *co_rcipher (void *a)
{
#ifdef DEBUG_COROUTINES
    debug ("co_rcipher(void *a) :: Invoked\n");
#endif
    char c, e;
    globals *g = a;
    while ((c = ring_pop (r_rcipher)) != EOF)
    {
        e = caesar_encrypt (c);
        ring_push (e, r_write);
        ring_push (e, r_wcount);
    }

    // Kill the writers
    ring_push (EOF, r_write);
    ring_push (EOF, r_wcount);

#ifdef DEBUG_COROUTINES
    debug ("co_rcipher(void *a) :: End-of-thread\n");
#endif

    pthread_exit (0);
}

void *co_wcount (void *a)
{
#ifdef DEBUG_COROUTINES
    debug ("co_wcount(void *a) :: Invoked\n");
#endif

    char c;
    globals *g = a;
    while ((c = ring_pop (r_wcount)) != EOF)
    {
        count_output (c);
    };

#ifdef DEBUG_COROUTINES
    debug ("co_wcount(void *a) :: End-of-thread\n");
#endif

    pthread_exit (0);
}

void *co_writer (void *a)
{
#ifdef DEBUG_COROUTINES
    debug ("co_writer(void *a) :: Invoked\n");
#endif

    char c;
    globals *g = a;
    while ((c = ring_pop (r_write)) != EOF)
    {
        write_output (c);
    };

    printf ("\nEnd of file reached.\n");
#ifdef DEBUG_COROUTINES
    debug ("co_writer(void *a) :: End-of-thread\n");
#endif

    pthread_exit (0);
}
