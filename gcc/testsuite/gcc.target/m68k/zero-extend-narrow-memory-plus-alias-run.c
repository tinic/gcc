/* { dg-do run } */
/* { dg-options "-O2 -m68000 -fomit-frame-pointer" } */
#define ATTR __attribute__((noinline, noclone))
ATTR unsigned long indexed_minus (const volatile unsigned char *p,
                                  unsigned long index)
{ return (unsigned char) (p[index] - 1); }
ATTR unsigned long post_plus (volatile unsigned char **pp)
{
  volatile unsigned char *p = *pp;
  unsigned long value = (unsigned char) (*p++ + 1);
  *pp = p;
  return value;
}
ATTR int byte_positive (const volatile unsigned char *p)
{
  unsigned long value = (unsigned char) (*p + 1);
  return (long) value > 0;
}
ATTR int word_positive (const volatile unsigned short *p)
{
  unsigned long value = (unsigned short) (*p + 1);
  return (long) value > 0;
}
static volatile unsigned char data[256];
static volatile unsigned short word;
int main (void)
{
  for (unsigned int i = 0; i < 256; i++)
    data[i] = (unsigned char) i;
  for (unsigned int i = 0; i < 256; i++)
    {
      unsigned long minus = i == 0 ? 255 : i - 1;
      unsigned long plus = i == 255 ? 0 : i + 1;
      volatile unsigned char *p = &data[i];
      if (indexed_minus (data, i) != minus
          || post_plus (&p) != plus || p != &data[i] + 1
          || byte_positive (&data[i]) != (i != 255))
        __builtin_abort ();
    }
  static const struct { unsigned short input; int expected; } cases[] =
    { {0,1}, {1,1}, {32766,1}, {32767,1}, {32768,1}, {65534,1}, {65535,0} };
  for (unsigned int i = 0; i < sizeof cases / sizeof cases[0]; i++)
    {
      word = cases[i].input;
      if (word_positive (&word) != cases[i].expected)
        __builtin_abort ();
    }
  return 0;
}
