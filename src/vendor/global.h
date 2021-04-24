#ifndef H_GLOBAL
#define H_GLOBAL
#include <pthread.h>

typedef struct
{
    char *readFrom;
    char *writeTo;
    unsigned in;
    unsigned out;
} globals;

#endif // H_GLOBAL
