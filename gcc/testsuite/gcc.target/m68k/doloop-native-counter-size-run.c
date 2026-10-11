/* { dg-do run } */
/* { dg-options "-Os -m68000 -fomit-frame-pointer -fno-tree-ch" } */
#define main counter_matrix_main
#include "doloop-native-counter-run.c"
#undef main

ATTR unsigned long narrow_limit_count (const volatile unsigned char *p,
                                      unsigned long n)
{
  if (n > 65534)
    n = 65534;
  unsigned long sum = 0;
  for (unsigned long i = 0; i < n; i++)
    sum += p[i];
  return sum;
}

/* Narrow only the compiler's counter, retaining the original argument
   and the full-width result when they are needed after the loop.  */
ATTR unsigned long keep_argument (const volatile unsigned char *p,
                                 unsigned long n)
{
  unsigned long limit = n > 511 ? 511 : n;
  unsigned long sum = 0;
  for (unsigned long i = 0; i < limit; i++)
    sum += p[i];
  return sum ^ n;
}

int main (void)
{
  static const unsigned long counts[] =
    { 0, 1, 511, 65534, 65535, 65536, 0x12345678UL, 0xffffffffUL };
  if (counter_matrix_main () != 0)
    __builtin_abort ();
  for (unsigned int i = 0; i < sizeof counts / sizeof counts[0]; i++)
    {
      unsigned long n = counts[i];
      if (keep_argument (data, n) != ((n > 511 ? 511 : n) ^ n)
          || narrow_limit_count (data, n) != (n > 65534 ? 65534 : n))
        __builtin_abort ();
    }
  return 0;
}
