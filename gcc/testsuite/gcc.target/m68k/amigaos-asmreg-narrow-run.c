/* { dg-do run { target *-*-amigaos* } } */
/* { dg-options "-O2 -m68000 -mregparm=3 -fomit-frame-pointer -fno-optimize-sibling-calls" } */

#define ATTR __attribute__((noinline, noclone))
ATTR static long sb (signed char x __asm("d0")) { return x; }
ATTR static unsigned long ub (unsigned char x __asm("d0")) { return x; }
ATTR static long sw (short x __asm("d0")) { return x; }
ATTR static unsigned long uw (unsigned short x __asm("d0")) { return x; }
ATTR static unsigned long d7 (unsigned char x __asm("d7")) { return x; }
ATTR static unsigned long mixed (const unsigned long *p __asm("a0"),
                                unsigned short x __asm("d2"),
                                unsigned char y __asm("d0"))
{
  return *p + x + y;
}

int main (void)
{
  static const unsigned long values[] = {
    0, 1, 127, 128, 255, 256, 32767, 32768,
    65535, 65536, 0x123480ff, 0x80000080, 0xffffffff
  };
  long (*signed_indirect) (signed char x __asm("d0")) = sb;
  unsigned long (*unsigned_indirect) (unsigned char x __asm("d0")) = ub;
  unsigned int i;
  for (i = 0; i < sizeof values / sizeof values[0]; ++i)
    {
      unsigned long v = values[i];
      volatile unsigned long volatile_v = v;
      if (sb (v) != (signed char) v || ub (v) != (unsigned char) v) return 1;
      if (sw (v) != (short) v || uw (v) != (unsigned short) v) return 2;
      if (d7 (v) != (unsigned char) v) return 3;
      if (mixed (&v, v, v) != v + (unsigned short) v + (unsigned char) v)
        return 4;
      if (signed_indirect (volatile_v) != (signed char) v
          || unsigned_indirect (volatile_v) != (unsigned char) v) return 5;
    }
  return 0;
}
