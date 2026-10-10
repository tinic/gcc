/* { dg-do compile } */
/* { dg-options "-O2 -m68000 -mregparm=3 -fomit-frame-pointer" } */

/* MOVEA does not set flags on the 68000.  A free data register can carry
   each loaded pointer's flags across the copy into its address register.  */
struct node { struct node *next; unsigned value; };

unsigned
null_load (struct node **slot)
{
  struct node *p = *slot;
  return p ? p->value : 0;
}

unsigned
null_volatile (struct node * volatile *slot, unsigned fallback)
{
  struct node *p = *slot;
  return p ? p->value : fallback;
}

unsigned
null_chain (struct node *p)
{
  unsigned total = 0;
  while ((p = p->next) != 0)
    total += p->value;
  return total;
}

/* { dg-final { scan-assembler-not {cmp\.w #0} } } */
/* { dg-final { scan-assembler-times {move\.l \(a[01]\),d[01]} 4 } } */
