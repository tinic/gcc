/* { dg-do run { target *-*-amigaos* } } */
/* { dg-options "-O2 -m68000 -mregparm=3 -fomit-frame-pointer -fno-optimize-sibling-calls -fno-defer-pop" } */

#define ATTR __attribute__((noinline, noclone))
static volatile unsigned long fills, consumes, checksum;

ATTR static void fill (unsigned char *b)
{
  unsigned int i;
  ++fills;
  for (i = 0; i < 64; ++i) b[i] = i ^ 0x5a;
  __asm__ volatile ("" ::: "d0", "d1", "a0", "a1");
}

ATTR static void consume (const unsigned char *b)
{
  unsigned long sum = 0;
  unsigned int i;
  ++consumes;
  for (i = 0; i < 64; ++i) sum += b[i];
  checksum = sum;
}

ATTR static void conditional (unsigned long x)
{
  unsigned char b[64];
  if (!x) return;
  fill (b); b[x & 63] = 1; consume (b);
}

ATTR static unsigned long sentinel (unsigned long x)
{
  unsigned char b[64];
  if (x == 42) return 0;
  fill (b);
  return b[x & 63];
}

ATTR static unsigned long zero (unsigned long x)
{
  unsigned char b[64];
  if (!x) return 0;
  fill (b);
  return b[x & 63];
}

int main (void)
{
  static const unsigned long values[] = { 0, 1, 42, 63, 64, 127, 0xffffffff };
  unsigned int i, j;
  for (i = 0; i < sizeof values / sizeof values[0]; ++i)
    {
      unsigned long x = values[i], sum = 0, before, after, result;
      fills = consumes = checksum = 0;
      __asm__ volatile ("move.l sp,%0" : "=d" (before));
      conditional (x);
      __asm__ volatile ("move.l sp,%0" : "=d" (after));
      if (before != after) return 1;
      if (!x)
        { if (fills || consumes || checksum) return 2; }
      else
        {
          for (j = 0; j < 64; ++j) sum += j == (x & 63) ? 1 : (j ^ 0x5a);
          if (fills != 1 || consumes != 1 || checksum != sum) return 3;
        }
      fills = 0;
      result = sentinel (x);
      if (fills != (x == 42 ? 0 : 1)
          || result != (x == 42 ? 0 : ((x & 63) ^ 0x5a))) return 4;
      fills = 0;
      result = zero (x);
      if (fills != (x ? 1 : 0)
          || result != (x ? ((x & 63) ^ 0x5a) : 0)) return 5;
    }
  return 0;
}
