/* { dg-do compile } */
/* { dg-options "-Os -m68000 -fomit-frame-pointer -fno-tree-ch" } */
/* Keep the test in the header: RTL must choose the narrow counter without
   depending on a dedicated IV introduced after header copying.  */
unsigned long bounded_size_counter (const volatile unsigned char *p,
                                   unsigned long n)
{
  if (n > 65534)
    n = 65534;
  unsigned long sum = 0;
  for (unsigned long i = 0; i < n; i++)
    sum += p[i];
  return sum;
}
/* { dg-final { scan-assembler "dbra" } } */
/* { dg-final { scan-assembler-not "clr\\.w" } } */
