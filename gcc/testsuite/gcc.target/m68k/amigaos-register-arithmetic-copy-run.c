/* { dg-do run { target *-*-amigaos* } } */
/* { dg-options "-O2 -m68000 -mregparm=3 -fomit-frame-pointer" } */

struct pair { unsigned long a, b; };
#define ATTR __attribute__((noinline, noclone))

ATTR static struct pair add_pair (unsigned long x, unsigned long y)
{
  struct pair p = { x + 1, y + 2 };
  return p;
}

/* The temporary remains live as the other half of the result.  */
ATTR static struct pair shared (unsigned long x)
{
  unsigned long t = x + 1;
  struct pair p = { t, t };
  return p;
}

/* Moving arithmetic must keep comparison flag tracking correct, including
   additions that overflow the signed range or carry out of bit 31.  */
ATTR static struct pair signed_branch (unsigned long x, unsigned long y)
{
  struct pair p = { x + 1, y + 2 };
  if ((long) p.b > 0) p.a ^= 0x55aa55aa;
  if ((long) p.b < 0) p.a ^= 0xaa55aa55;
  return p;
}

ATTR static struct pair zero_branch (unsigned long x, unsigned long y)
{
  struct pair p = { x + 1, y + 2 };
  if (p.b == 0) p.a ^= 0x12345678;
  if (p.b > 127) p.a ^= 0x87654321;
  return p;
}

int main (void)
{
  static const unsigned long values[] = {
    0, 1, 7, 127, 128, 0x7ffffffd, 0x7ffffffe,
    0x7fffffff, 0x80000000, 0xfffffffd, 0xfffffffe, 0xffffffff
  };
  unsigned int i, j;
  for (i = 0; i < sizeof values / sizeof values[0]; ++i)
    {
      struct pair s = shared (values[i]);
      if (s.a != values[i] + 1 || s.b != s.a) return 1;
      for (j = 0; j < sizeof values / sizeof values[0]; ++j)
        {
          unsigned long a = values[i] + 1, b = values[j] + 2;
          struct pair p = add_pair (values[i], values[j]);
          if (p.a != a || p.b != b) return 2;
          p = signed_branch (values[i], values[j]);
          if ((long) b > 0) a ^= 0x55aa55aa;
          if ((long) b < 0) a ^= 0xaa55aa55;
          if (p.a != a || p.b != b) return 3;
          a = values[i] + 1;
          p = zero_branch (values[i], values[j]);
          if (b == 0) a ^= 0x12345678;
          if (b > 127) a ^= 0x87654321;
          if (p.a != a || p.b != b) return 4;
        }
    }
  return 0;
}
