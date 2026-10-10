/* { dg-do compile } */
/* { dg-options "-O2 -m68000 -mregparm=3 -fomit-frame-pointer" } */

/* D0 and D1 are both live across the test; do not clobber either as a
   scratch, or introduce a write to an otherwise unused saved register.  */
struct node { struct node *next; unsigned value; };
unsigned
null_pressure (struct node **slot, unsigned fallback, unsigned extra)
{
  struct node *p = *slot;
  if (!p)
    return fallback ^ extra;
  return p->value + fallback + extra;
}

/* { dg-final { scan-assembler {cmp\.w #0,a0} } } */
/* { dg-final { scan-assembler-not {movem|d[2-7]} } } */
