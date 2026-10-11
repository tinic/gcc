/* { dg-do run } */
/* { dg-options "-O2 -m68000 -fomit-frame-pointer" } */
static unsigned char data[65538];
#define ATTR __attribute__((noinline, noclone))
ATTR unsigned long small_count (const volatile unsigned char *p, unsigned long n)
{
  if (n > 511)
    n = 511;
  unsigned long sum = 0;
  for (unsigned long i = 0; i < n; i++)
    sum += p[i];
  return sum;
}
ATTR unsigned long limit_count (const volatile unsigned char *p, unsigned long n)
{
  if (n > 65535)
    n = 65535;
  unsigned long sum = 0;
  for (unsigned long i = 0; i < n; i++)
    sum += p[i];
  return sum;
}
ATTR unsigned long wide_count (const volatile unsigned char *p, unsigned long n)
{
  unsigned long sum = 0;
  for (unsigned long i = 0; i < n; i++)
    sum += p[i];
  return sum;
}
int main (void)
{
  static const unsigned long counts[] =
    { 0, 1, 2, 255, 256, 510, 511, 512, 65534, 65535, 65536, 65537, 65538 };
  for (unsigned long i = 0; i < sizeof data; i++)
    data[i] = 1;
  for (unsigned int i = 0; i < sizeof counts / sizeof counts[0]; i++)
    {
      unsigned long n = counts[i];
      if (small_count (data, n) != (n > 511 ? 511 : n)
          || limit_count (data, n) != (n > 65535 ? 65535 : n)
          || wide_count (data, n) != n)
        __builtin_abort ();
    }
  return 0;
}
