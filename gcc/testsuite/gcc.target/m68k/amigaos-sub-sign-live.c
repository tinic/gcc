/* { dg-do compile } */
/* { dg-options "-O2 -m68000 -mregparm=3 -fomit-frame-pointer -fno-optimize-sibling-calls" } */

/* A live difference must still be computed, and its SUB flags reused. */
extern void side (unsigned);
unsigned
wrap_reused (unsigned x, unsigned y)
{
  unsigned delta = x - y;
  if ((int) delta < 0)
    side (delta);
  return delta;
}

/* { dg-final { scan-assembler-times {sub\.l} 1 } } */
/* { dg-final { scan-assembler-not {cmp\.l|cmp\.w #0} } } */
