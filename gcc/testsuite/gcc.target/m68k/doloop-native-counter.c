/* { dg-do compile } */
/* { dg-options "-O2 -m68000 -fomit-frame-pointer" } */
unsigned long bounded_counter (const volatile unsigned char *p,
                               unsigned long n)
{
  if (n > 511)
    n = 511;
  unsigned long sum = 0;
  for (unsigned long i = 0; i < n; i++)
    sum += p[i];
  return sum;
}
/* No long-count rollover sequence is needed for the proven small bound.  */
/* { dg-final { scan-assembler "dbra" } } */
/* { dg-final { scan-assembler-not "clr\\.w" } } */
