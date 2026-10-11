/* { dg-do run { target *-*-amigaos* } } */
/* { dg-options "-O2 -m68000 -mregparm=3 -fomit-frame-pointer" } */

#define FN(N) \
  __attribute__((noinline, noclone)) \
  static unsigned long store##N (unsigned long x, \
                                 volatile unsigned char *p) \
  { *p = x >> N; return x; }
FN(16) FN(17) FN(18) FN(19) FN(20) FN(21) FN(22) FN(23) FN(24)

__attribute__((noinline, noclone))
static unsigned long live (unsigned long x, volatile unsigned char *p)
{
  unsigned long t = x >> 16;
  *p = t;
  return t;
}

__attribute__((noinline, noclone))
static void indexed (unsigned long x, unsigned char *p)
{
  unsigned long t = x >> 16;
  p[t] = t;
}

int main (void)
{
  static const unsigned long inputs[] = {
    0, 1, 0xff, 0xffff, 0x10000, 0x12345678,
    0x80000000, 0xff000001, 0xffff0000, 0xffffffff
  };
  volatile unsigned char out[3];
  unsigned char indexed_out[256];
  unsigned int i;
  for (i = 0; i < sizeof inputs / sizeof inputs[0]; ++i)
    {
      unsigned long x = inputs[i];
      out[0] = 0x5a; out[1] = 0xa5; out[2] = 0x96;
#define CHECK(N) \
      if (store##N (x, &out[1]) != x \
          || out[1] != (unsigned char) (x >> N) \
          || out[0] != 0x5a || out[2] != 0x96) return N;
      CHECK(16) CHECK(17) CHECK(18) CHECK(19) CHECK(20)
      CHECK(21) CHECK(22) CHECK(23) CHECK(24)
      if (live (x, &out[1]) != x >> 16
          || out[1] != (unsigned char) (x >> 16)) return 25;
    }
  for (i = 0; i < 256; ++i) indexed_out[i] = 0xa5;
  indexed (0x007f80ff, indexed_out);
  for (i = 0; i < 256; ++i)
    if (indexed_out[i] != (i == 127 ? 127 : 0xa5)) return 26;
  return 0;
}
