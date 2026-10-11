/* { dg-do run } */
/* { dg-options "-Os -m68000 -fomit-frame-pointer -fno-tree-ch" } */
#include "doloop-long-counter-size.c"
static unsigned char data[65538];
int main (void)
{
  /* Independent golden sums cover low-word rollover and wide inputs. */
  static const struct { unsigned long count, sum; } cases[] =
    {
      { 0UL, 0x00000000UL },
      { 1UL, 0x00000013UL },
      { 2UL, 0x0000004bUL },
      { 255UL, 0x00007e92UL },
      { 256UL, 0x00007f80UL },
      { 510UL, 0x0000fd49UL },
      { 511UL, 0x0000fe12UL },
      { 512UL, 0x0000ff00UL },
      { 65534UL, 0x007f7e49UL },
      { 65535UL, 0x007f7f12UL },
      { 65536UL, 0x007f8000UL },
      { 65537UL, 0x007f8013UL },
      { 65538UL, 0x007f804bUL },
      { 0xfffffffeUL, 0x007f804bUL },
      { 0xffffffffUL, 0x007f804bUL }
    };
  for (unsigned long i = 0; i < sizeof data; i++)
    data[i] = (unsigned char) (i * 37UL + 19UL);
  for (unsigned int i = 0; i < sizeof cases / sizeof cases[0]; i++)
    if (long_size_sum (data, cases[i].count) != cases[i].sum)
      __builtin_abort ();
  return 0;
}
