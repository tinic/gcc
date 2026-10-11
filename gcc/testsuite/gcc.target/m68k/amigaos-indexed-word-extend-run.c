/* { dg-do run { target *-*-amigaos* } } */
/* { dg-options "-O2 -m68000 -mregparm=3 -fomit-frame-pointer" } */

#define ATTR __attribute__((noinline, noclone))
ATTR unsigned long word_at (const unsigned short *p, unsigned long i)
{
  return p[i];
}
ATTR unsigned long volatile_word_at (const volatile unsigned short *p,
                                     unsigned long i)
{
  return p[i];
}
ATTR unsigned long busy (const unsigned short *p, unsigned long i,
                         unsigned long x __asm("d1"))
{
  unsigned long y = p[i];
  __asm volatile ("" : "+d" (x) : "d" (y));
  return y + x;
}
ATTR unsigned long conditional (const unsigned short *p, unsigned long i)
{
  unsigned long y = p[i];
  return y ? y + 123 : 7;
}

int main (void)
{
  static const unsigned short words[] = {
    0, 1, 127, 128, 255, 256, 32767, 32768, 65535
  };
  static const unsigned long live[] = { 0, 1, 0x12345678, 0xffffffff };
  for (unsigned int i = 0; i < sizeof words / sizeof words[0]; i++)
    {
      unsigned long y = words[i];
      if (word_at (words, i) != y || volatile_word_at (words, i) != y
          || conditional (words, i) != (y ? y + 123 : 7))
        __builtin_abort ();
      for (unsigned int j = 0; j < sizeof live / sizeof live[0]; j++)
        if (busy (words, i, live[j]) != y + live[j])
          __builtin_abort ();
    }
  return 0;
}
