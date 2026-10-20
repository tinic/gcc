/* Runtime coverage for the 68000 size optimizations: widening multiply,
   full-width loop counters, and bounded switch indexes.  The AmigaOS
   driver supplies the optimization, LTO, allocator and CPU matrix.  */
/* { dg-do run } */

#define TEST_FN __attribute__ ((noipa))
#include "os-size-runtime.h"
