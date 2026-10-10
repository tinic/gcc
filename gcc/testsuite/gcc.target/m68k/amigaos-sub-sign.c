/* { dg-do compile } */
/* { dg-options "-O2 -m68000 -mregparm=3 -fomit-frame-pointer -fno-optimize-sibling-calls" } */

/* These subtract results are used only to choose a branch.  The flags
   can be produced by CMP without allocating a result register.  */
extern void side (unsigned);
unsigned wrap_negative (unsigned x, unsigned y)
{
  if ((int) (x - y) < 0) side (x);
  return x + y;
}
unsigned wrap_positive (unsigned x, unsigned y)
{
  if ((int) (x - y) >= 0) side (y);
  return x ^ y;
}
unsigned wrap_pointer (unsigned x, void *y)
{
  if ((int) (x - (unsigned) y) < 0) side (x);
  return x + (unsigned) y;
}

/* { dg-final { scan-assembler-times {cmp\.l} 3 } } */
/* { dg-final { scan-assembler-not {sub\.l|cmp\.w #0} } } */
