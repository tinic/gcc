/* { dg-do compile } */
/* { dg-options "-O2 -m68000 -fomit-frame-pointer" } */
/* The pointer-distance doloop count should simplify in generic RTL,
   without a target peephole or a change to niter copy-propagation policy.  */
short bounded_zero (const char *text, unsigned long size)
{
  for (unsigned long i = 0; i < size; i++)
    if (text[i] == 0)
      return 1;
  return 0;
}
/* { dg-final { scan-assembler-not "not\\.l" } } */
