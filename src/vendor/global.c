#include "global.h"
#include <stdlib.h>

globals *ctor_global ()
{
    return calloc (1, sizeof (globals));
}

void dtor_global (globals *g)
{
    free (g);
}

