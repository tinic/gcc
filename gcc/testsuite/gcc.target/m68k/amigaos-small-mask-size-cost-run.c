/* { dg-do run { target *-*-amigaos* } } */
/* { dg-options "-Os -m68000 -mregparm=3 -fomit-frame-pointer" } */

#define ATTR __attribute__((noinline, noclone))
ATTR unsigned long extract (unsigned long x)
{
  return ((x >> 5) & 7) | ((x >> 17) & 31);
}
ATTR unsigned long mask7 (unsigned long x) { return x & 7; }
ATTR unsigned long mask31 (unsigned long x) { return x & 31; }
ATTR unsigned long mask127 (unsigned long x) { return x & 127; }
ATTR unsigned long mask128 (unsigned long x) { return x & 128; }
ATTR unsigned long negative_mask (unsigned long x) { return x & -32L; }
ATTR unsigned long reference (unsigned long x, unsigned long n,
                              unsigned long mask)
{
  return (x >> n) & mask;
}
ATTR unsigned long live (unsigned long x, unsigned long y)
{
  return ((x >> 5) & 7) + ((y >> 17) & 31) + x + y;
}
ATTR unsigned long conditional (unsigned long x)
{
  unsigned long y = x & 31;
  return y ? y + 100 : 7;
}

int main (void)
{
  static const unsigned long values[] = {
    0, 1, 7, 8, 31, 32, 127, 128, 255, 256, 65535, 65536,
    0x7fffffff, 0x80000000, 0xff000001, 0x12345678, 0xffffffff
  };
  for (unsigned int i = 0; i < sizeof values / sizeof values[0]; i++)
    {
      unsigned long x = values[i];
      if (extract (x) != (reference (x, 5, 7) | reference (x, 17, 31))
          || mask7 (x) != reference (x, 0, 7)
          || mask31 (x) != reference (x, 0, 31)
          || mask127 (x) != reference (x, 0, 127)
          || mask128 (x) != reference (x, 0, 128)
          || negative_mask (x) != reference (x, 0, -32L))
        __builtin_abort ();
      unsigned long y = reference (x, 0, 31);
      if (conditional (x) != (y ? y + 100 : 7))
        __builtin_abort ();
      for (unsigned int j = 0; j < sizeof values / sizeof values[0]; j++)
        if (live (x, values[j])
            != reference (x, 5, 7) + reference (values[j], 17, 31)
               + x + values[j])
          __builtin_abort ();
    }
  return 0;
}
