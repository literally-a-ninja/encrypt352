#include "coroutines.h"
#include "common.h"
#include "ring.h"
#include <pthread.h>

#define DEBUG_COROUTINES

ring *rg_read;

void *co_coordinator (void *a)
{

#ifdef DEBUG_COROUTINES
    printf ("D: co_coordinator(void *a) :: Invoked\n");
#endif

    pthread_t t [] = {0};

    rg_read = ctor_ring (2);

    pthread_create (&t [0], NULL, &co_reader, a);
    pthread_create (&t [1], NULL, co_rcount, a);
    // pthread_create (threads [2], NULL, co_rcipher, a);
    //

    pthread_join (t [0], 0);

#ifdef DEBUG_COROUTINES
    printf ("D: co_coordinator(void *a) :: End-of-thread\n");
#endif

    pthread_exit (0);

    //    char c;
    //    while ((c = read_input ()) != EOF)
    //    {
    //        count_input (c);
    //        c = caesar_encrypt (c);
    //        count_output (c);
    //        write_output (c);
    //    }
    //
    //    pthread_exit (0);
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
        ring_push (c, rg_read);
    } while (EOF != c);

#ifdef DEBUG_COROUTINES
    printf ("D: co_reader(void *a) :: End-of-thread\n");
#endif

    pthread_exit (0);
}

void *co_rcount (void *a)
{
    char c;
    do
    {
        c = ring_pop (rg_read);
        printf ("%c\n", c);
    } while (EOF != c);

    pthread_exit (0);
}

void *co_rcipher (void *a)
{
    // Unimplemented
    pthread_exit (0);
}
