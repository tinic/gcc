/* { dg-do compile } */
/* { dg-options "-Os -m68000 -fomit-frame-pointer -fno-tree-ch" } */
#include "doloop-native-counter-large.c"

unsigned long at_limit_counter (const volatile unsigned char *p,
                               unsigned long n)
{
  if (n > 65535)
    n = 65535;
  unsigned long sum = 0;
  for (unsigned long i = 0; i < n; i++)
    sum += p[i];
  return sum;
}
/* Retain a long counter with no proven bound or no headroom for a possible
   count increment in the preferred mode.  */
/* { dg-final { scan-assembler-times "(clr\\.w|cmpa\\.w #-1)" 2 } } */
