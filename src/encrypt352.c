#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h> // bool type
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h> // stat

#include "fkworks0/debug.h"
#include "vendor/commmon.h"

globals *g;

int file_exists (char *filename)
{
    struct stat buffer;
    return (stat (filename, &buffer) == 0);
}

char *find_argument (int index, int argc, char *argv [])
{
    unsigned i;
    int occurances = index;
    for (i = 0; i < argc; i++)
    {
        char *s      = argv [i];
        int bIsShort = *s == '-';
        if (!bIsShort && !occurances--)
            return s;
    }

    return 0;
}

int find_short_argument (char c, int argc, char *argv [])
{
    unsigned i;
    for (i = 0; i < argc; i++)
    {
        char *s      = argv [i];
        int bIsShort = *s && *(s + 1);
        if (*(s + 1) == c)
            return 1;
    }

    return 0;
}

int pre_malformed_input (int argc, char *argv [])
{
    char *readFile  = find_argument (1, argc, argv);
    char *writeFile = find_argument (2, argc, argv);

    if (!(readFile && writeFile))
    {
        // clang-format off
        fprintf (stderr, 
            "Usage: %s [OPTIONS]... READ_FROM WRITE_TO"                     "\n\n"
            "Options"                                                       "\n"
            "  -f\t"    "Auto-acknowledge overwriting of WRITE_TO file."
        "\n\n",*argv);
        // clang-format on
        return 1;
    }

    if (!file_exists (readFile))
    {
        fprintf (stderr, "E: Input file does not exist.\n\n");
        return 1;
    }

    if (file_exists (writeFile) && !find_short_argument ('f', argc, argv))
    {
        printf ("%s: Confirm overwrite of file '%s' [Y/n]: ", *argv, writeFile);
        char r;
        scanf ("%c", &r);

        if (!(r == 'y' || r == 'Y' || r == '\n'))
            return 1;
    }

    return 0;
}

int pre_buffers (unsigned *in, unsigned *out)
{
    printf ("Read buffer capacity  : [1 < n] ");
    scanf ("%u", in);
    if (*in < 2)
    {
        fprintf (stderr, "E: Bounded amount; capacity must be "
                         "greater than one.\n\n");
        return 1;
    }

    printf ("Write buffer capacity : [1 < n] ");
    scanf ("%u", out);
    if (*out < 2)
    {
        fprintf (stderr, "E: Bounded amount; Capacity must be "
                         "greater than one.\n\n");
    }

    return 0;
}

int main (int argc, char *argv [])
{
#ifdef DEBUG_COROUTINES
    debug ("main :: Invoked");
#endif
    g = malloc (sizeof (globals));

    if (pre_malformed_input (argc, argv))
        return 0;

    if (pre_buffers (&g->in, &g->out))
        return 0;

    g->readFrom = find_argument (1, argc, argv);
    g->writeTo  = find_argument (2, argc, argv);

    coordinator = ctor_coordinator (g);
    pthread_join (*coordinator, NULL);

#ifdef DEBUG_COROUTINES
    debug ("main :: End-of-thread");
#endif

    return 0;
}
