/* { dg-do run { target *-*-amigaos* } } */
/* { dg-options "-O2 -m68000 -mregparm=3 -fomit-frame-pointer" } */

#define ATTR __attribute__((noinline, noclone))
#define CASE(N) \
  ATTR unsigned long shift##N (unsigned long x) { return (x >> N) & 31; }
CASE (16) CASE (17) CASE (18) CASE (19) CASE (20)
CASE (21) CASE (22) CASE (23) CASE (24)

ATTR unsigned long reference (unsigned long x, unsigned long n)
{
  return (x >> n) & 31;
}

ATTR unsigned long shared (unsigned long x, volatile unsigned long *p)
{
  x >>= 17;
  *p = x;
  return x & 31;
}

ATTR unsigned long conditional (unsigned long x)
{
  x = (x >> 17) & 31;
  return x ? x + 100 : 7;
}

int main (void)
{
  static const unsigned long values[] = {
    0, 1, 65535, 65536, 0x1ffff, 0x20000, 0x3ffff,
    0x7fffffff, 0x80000000, 0xff000001, 0x12345678, 0xffffffff
  };
  unsigned long (*volatile f[])(unsigned long) = {
    shift16, shift17, shift18, shift19, shift20,
    shift21, shift22, shift23, shift24
  };
  for (unsigned int i = 0; i < sizeof values / sizeof values[0]; i++)
    {
      unsigned long x = values[i];
      for (unsigned int j = 0; j < 9; j++)
        if (f[j] (x) != reference (x, j + 16))
          __builtin_abort ();
      volatile unsigned long stored = 0;
      if (shared (x, &stored) != reference (x, 17) || stored != (x >> 17))
        __builtin_abort ();
      unsigned long y = reference (x, 17);
      if (conditional (x) != (y ? y + 100 : 7))
        __builtin_abort ();
    }
  return 0;
}
