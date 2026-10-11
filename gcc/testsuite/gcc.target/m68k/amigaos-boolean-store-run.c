/* { dg-do run { target *-*-amigaos* } } */
/* { dg-options "-O2 -m68000 -mregparm=3 -fomit-frame-pointer" } */

#define ATTR __attribute__((noinline, noclone))
#define CASE(NAME, TYPE, OP) \
  ATTR unsigned long NAME (TYPE x __asm("d1"), TYPE y __asm("d2")) \
  { return x OP y; }
CASE (equal8, unsigned char, ==)
CASE (less8, signed char, <)
CASE (equal16, unsigned short, ==)
CASE (less16, signed short, <)
CASE (equal32, unsigned long, ==)
CASE (less32, signed long, <)
CASE (less_unsigned, unsigned long, <)
CASE (less_equal32, signed long, <=)
CASE (greater32, signed long, >)
CASE (greater_equal32, signed long, >=)
ATTR unsigned long overlap (unsigned long x, unsigned long y) { return x == y; }
ATTR unsigned long equal_mem (const volatile unsigned char *p,
                              unsigned char y __asm("d1"))
{ return *p == y; }

int main (void)
{
  static const unsigned long values[] = {
    0, 1, 127, 128, 255, 256, 32767, 32768, 65535, 65536,
    0x7fffffff, 0x80000000, 0xffffffff
  };
  for (unsigned int i = 0; i < sizeof values / sizeof values[0]; i++)
    for (unsigned int j = 0; j < sizeof values / sizeof values[0]; j++)
      {
        unsigned long x = values[i], y = values[j];
        volatile unsigned char byte = x;
        if (equal8 (x, y) != ((unsigned char)x == (unsigned char)y)
            || less8 (x, y) != ((signed char)x < (signed char)y)
            || equal16 (x, y) != ((unsigned short)x == (unsigned short)y)
            || less16 (x, y) != ((signed short)x < (signed short)y)
            || equal32 (x, y) != (x == y)
            || less32 (x, y) != ((signed long)x < (signed long)y)
            || less_unsigned (x, y) != (x < y)
            || less_equal32 (x, y) != ((signed long)x <= (signed long)y)
            || greater32 (x, y) != ((signed long)x > (signed long)y)
            || greater_equal32 (x, y) != ((signed long)x >= (signed long)y)
            || overlap (x, y) != (x == y)
            || equal_mem (&byte, y) != ((unsigned char)x == (unsigned char)y))
          __builtin_abort ();
      }
  return 0;
}
