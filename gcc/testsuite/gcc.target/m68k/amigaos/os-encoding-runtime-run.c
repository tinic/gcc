/* { dg-do run } */
/* The AmigaOS driver supplies optimization, LTO and allocator variants.  */
#define TEST_FN __attribute__ ((noipa))
#include "os-encoding-runtime.h"
