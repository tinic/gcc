/* { dg-do run } */
/* { dg-options "-Os -m68000 -fomit-frame-pointer" } */
#include "doloop-preserve-flags.c"
static unsigned long input[512];
static volatile unsigned long output[513];
int main (void)
{
  /* Expected sums calculated independently modulo 2^32.  */
  static const struct { unsigned long count, value, sum; } cases[] =
    {
      { 0x00000000UL, 0x00000000UL, 0x00000000UL },
      { 0x00000000UL, 0x00000001UL, 0x00000001UL },
      { 0x00000000UL, 0x00000100UL, 0x00000100UL },
      { 0x00000000UL, 0x7fffffffUL, 0x7fffffffUL },
      { 0x00000000UL, 0x80000000UL, 0x80000000UL },
      { 0x00000000UL, 0xffffffffUL, 0xffffffffUL },
      { 0x00000001UL, 0x00000000UL, 0x00000000UL },
      { 0x00000001UL, 0x00000001UL, 0x00000002UL },
      { 0x00000001UL, 0x00000100UL, 0x00000200UL },
      { 0x00000001UL, 0x7fffffffUL, 0xfffffffeUL },
      { 0x00000001UL, 0x80000000UL, 0x00000000UL },
      { 0x00000001UL, 0xffffffffUL, 0xfffffffeUL },
      { 0x000000ffUL, 0x00000000UL, 0x00000000UL },
      { 0x000000ffUL, 0x00000001UL, 0x00000100UL },
      { 0x000000ffUL, 0x00000100UL, 0x00010000UL },
      { 0x000000ffUL, 0x7fffffffUL, 0xffffff00UL },
      { 0x000000ffUL, 0x80000000UL, 0x00000000UL },
      { 0x000000ffUL, 0xffffffffUL, 0xffffff00UL },
      { 0x00000100UL, 0x00000000UL, 0x00000000UL },
      { 0x00000100UL, 0x00000001UL, 0x00000101UL },
      { 0x00000100UL, 0x00000100UL, 0x00010100UL },
      { 0x00000100UL, 0x7fffffffUL, 0x7ffffeffUL },
      { 0x00000100UL, 0x80000000UL, 0x80000000UL },
      { 0x00000100UL, 0xffffffffUL, 0xfffffeffUL },
      { 0x000001ffUL, 0x00000000UL, 0x00000000UL },
      { 0x000001ffUL, 0x00000001UL, 0x00000200UL },
      { 0x000001ffUL, 0x00000100UL, 0x00020000UL },
      { 0x000001ffUL, 0x7fffffffUL, 0xfffffe00UL },
      { 0x000001ffUL, 0x80000000UL, 0x00000000UL },
      { 0x000001ffUL, 0xffffffffUL, 0xfffffe00UL },
      { 0x00000200UL, 0x00000000UL, 0x00000000UL },
      { 0x00000200UL, 0x00000001UL, 0x00000001UL },
      { 0x00000200UL, 0x00000100UL, 0x00000100UL },
      { 0x00000200UL, 0x7fffffffUL, 0x7fffffffUL },
      { 0x00000200UL, 0x80000000UL, 0x80000000UL },
      { 0x00000200UL, 0xffffffffUL, 0xffffffffUL },
      { 0x0000ffffUL, 0x00000000UL, 0x00000000UL },
      { 0x0000ffffUL, 0x00000001UL, 0x00000200UL },
      { 0x0000ffffUL, 0x00000100UL, 0x00020000UL },
      { 0x0000ffffUL, 0x7fffffffUL, 0xfffffe00UL },
      { 0x0000ffffUL, 0x80000000UL, 0x00000000UL },
      { 0x0000ffffUL, 0xffffffffUL, 0xfffffe00UL },
      { 0x00010000UL, 0x00000000UL, 0x00000000UL },
      { 0x00010000UL, 0x00000001UL, 0x00000001UL },
      { 0x00010000UL, 0x00000100UL, 0x00000100UL },
      { 0x00010000UL, 0x7fffffffUL, 0x7fffffffUL },
      { 0x00010000UL, 0x80000000UL, 0x80000000UL },
      { 0x00010000UL, 0xffffffffUL, 0xffffffffUL },
      { 0xffffffffUL, 0x00000000UL, 0x00000000UL },
      { 0xffffffffUL, 0x00000001UL, 0x00000200UL },
      { 0xffffffffUL, 0x00000100UL, 0x00020000UL },
      { 0xffffffffUL, 0x7fffffffUL, 0xfffffe00UL },
      { 0xffffffffUL, 0x80000000UL, 0x00000000UL },
      { 0xffffffffUL, 0xffffffffUL, 0xfffffe00UL }
    };
  for (unsigned int i = 0; i < sizeof cases / sizeof cases[0]; i++)
    {
      unsigned long n = cases[i].count;
      unsigned long value = cases[i].value;
      unsigned long sum = cases[i].sum;
      unsigned long count = (n & 511) + 1;
      for (unsigned int j = 0; j < 512; j++)
        input[j] = value;
      output[count] = 0x13579bdfUL;
      if (fill_zero (output, n, value) != (value == 0)
          || output[0] != value || output[count - 1] != value
          || output[count] != 0x13579bdfUL
          || sum_zero (input, n) != (sum == 0)
          || sum_positive (input, n) != (sum != 0 && sum < 0x80000000UL))
        __builtin_abort ();
    }
  return 0;
}
