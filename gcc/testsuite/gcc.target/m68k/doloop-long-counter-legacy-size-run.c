/* { dg-do run } */
/* { dg-options "-Os -m68000 -fomit-frame-pointer -fno-branch-count-reg" } */
#include "doloop-long-counter-legacy-size.c"
int main (void)
{
  static const unsigned long counts[] =
    { 0, 1, 2, 255, 256, 510, 511, 512,
      65534, 65535, 65536, 65537, 65538 };
  volatile unsigned long last;
  for (unsigned int i = 0; i < sizeof counts / sizeof counts[0]; i++)
    {
      last = 0xdeadbeefUL;
      if (decrement_visible (counts[i], &last) != 0xffffffffUL || last != 0)
        __builtin_abort ();
      last = 0xdeadbeefUL;
      if (decrement_signed_visible ((long) counts[i], (volatile long *) &last)
          != -1L || last != 0)
        __builtin_abort ();
    }
  static const long negatives[] = { -1L, -2L, -2147483647L - 1L };
  for (unsigned int i = 0; i < sizeof negatives / sizeof negatives[0]; i++)
    {
      last = 0xdeadbeefUL;
      if (decrement_signed_visible (negatives[i], (volatile long *) &last)
          != negatives[i] || last != 0xdeadbeefUL)
        __builtin_abort ();
    }
  return 0;
}
