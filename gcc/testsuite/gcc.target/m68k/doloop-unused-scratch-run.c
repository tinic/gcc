/* { dg-do run } */
/* { dg-options "-O2 -m68000 -fomit-frame-pointer -fno-tree-ch" } */
#include "doloop-unused-scratch.c"
static unsigned long data[65538];
int main (void)
{
  /* Golden sums computed independently with 32-bit unsigned arithmetic.  */
  static const struct { unsigned long count, sum; } cases[] =
    {
      { 0, 0x00000000UL },
      { 1, 0x12345678UL },
      { 2, 0x2469acf1UL },
      { 255, 0xa0a3a009UL },
      { 256, 0xb3d6f780UL },
      { 510, 0x3f493e13UL },
      { 511, 0x537b9689UL },
      { 512, 0x67aeef00UL },
      { 65534, 0x320fd313UL },
      { 65535, 0x44432989UL },
      { 65536, 0x56778000UL },
      { 65537, 0x68acd678UL },
      { 65538, 0x7ae32cf1UL }
    };
  for (unsigned long i = 0; i < sizeof data / sizeof data[0]; i++)
    data[i] = i * 65537UL + 0x12345678UL;
  for (unsigned int i = 0; i < sizeof cases / sizeof cases[0]; i++)
    {
      unsigned long n = cases[i].count;
      unsigned long bounded = n > 511 ? 0x537b9689UL : cases[i].sum;
      if (bounded_word_sum (data, n) != bounded
          || wide_word_sum (data, n) != cases[i].sum)
        __builtin_abort ();
    }
  return 0;
}
