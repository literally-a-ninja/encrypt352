#pragma once

#include "ansicolors.h"
#include <error.h>
#include <stdio.h>

/**
 * Debug helpers from anacrolix/archive on GitHub.
 *
 **/

#define static_assert(cond)                                                    \
    __attribute__ ((unused)) extern char dummy_assert_array [(cond) ? 1 : -1]

#define verify(f) (assert (f))

#define warn(errval, fmt, ...)                                                 \
    (error_at_line (0, errval, __FILE__, __LINE__,                             \
                    ANSI_COLOR_YELLOW "W: " ANSI_COLOR_RESET fmt,              \
                    ##__VA_ARGS__))

#define fatal(errval, fmt, ...)                                                \
    (error_at_line (EXIT_FAILURE, errval, __FILE__, __LINE__,                  \
                    ANSI_COLOR_RED "E: " ANSI_COLOR_RESET fmt, ##__VA_ARGS__))

#define debug(fmt, ...)                                                        \
    fprintf (stderr, ANSI_COLOR_BLUE "D: " ANSI_COLOR_RESET);                  \
    fprintf (stderr, fmt "\n", ##__VA_ARGS__)

#define dump(varname) debug ("%s = %d (0x%02X)", #varname, varname, varname);

#define psize(type)                                                            \
    (error_at_line (0, 0, __FILE__, __LINE__, "sizeof(" #type ") : %lu",       \
                    sizeof (type)))

#define trace(fmt, var)                                                        \
    (error_at_line (0, 0, __FILE__, __LINE__, "%s : " fmt, #var, var))

