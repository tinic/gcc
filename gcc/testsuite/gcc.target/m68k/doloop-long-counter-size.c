/* { dg-do compile } */
/* { dg-options "-Os -m68000 -fomit-frame-pointer -fno-tree-ch" } */
#define ATTR __attribute__((noinline, noclone))
ATTR unsigned long long_size_sum (const volatile unsigned char *p,
                                 unsigned long n)
{
  if (n > 65538)
    n = 65538;
  unsigned long sum = 0;
  for (unsigned long i = 0; i < n; i++)
    sum += p[i];
  return sum;
}
/* An SI counter must retain a full-width decrement; it needs no software
   DBRA rollover sequence when optimizing size.  */
/* { dg-final { scan-assembler "subq\\.l #1" } } */
/* { dg-final { scan-assembler-not "(dbra|clr\\.w)" } } */
