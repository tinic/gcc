/* { dg-do compile } */
/* { dg-options "-O2 -m68000 -fomit-frame-pointer" } */
unsigned long wide_counter (const volatile unsigned char *p, unsigned long n)
{
  unsigned long sum = 0;
  for (unsigned long i = 0; i < n; i++)
    sum += p[i];
  return sum;
}
/* The unknown count must retain the long data-register rollover or
   long address-register decrement/comparison sequence.  */
/* { dg-final { scan-assembler "(clr\\.w|cmpa\\.w #-1)" } } */
