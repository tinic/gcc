/* { dg-do compile } */
/* { dg-options "-O2 -m68000 -fomit-frame-pointer -fno-tree-ch" } */
#define ATTR __attribute__((noinline, noclone))
ATTR unsigned long bounded_word_sum (const volatile unsigned long *p,
                                    unsigned long n)
{
  if (n > 511)
    n = 511;
  unsigned long sum = 0;
  for (unsigned long i = 0; i < n; i++)
    sum += *p++;
  return sum;
}
ATTR unsigned long wide_word_sum (const volatile unsigned long *p,
                                 unsigned long n)
{
  unsigned long sum = 0;
  for (unsigned long i = 0; i < n; i++)
    sum += *p++;
  return sum;
}
/* The body and counters fit in caller-clobbered registers.  A dummy
   scratch must not force an additional callee-saved register.  */
/* { dg-final { scan-assembler "dbra" } } */
/* { dg-final { scan-assembler-not "(movem|move\\.l [da][2-7],-\\(sp\\))" } } */
