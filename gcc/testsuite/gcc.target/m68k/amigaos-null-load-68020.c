/* { dg-do compile } */
/* { dg-options "-O2 -m68020 -mregparm=3 -fomit-frame-pointer" } */

/* TST on an address register already has a short encoding on the 68020.
   The 68000 load-through-data peephole must not add an instruction here.  */
struct node { struct node *next; unsigned value; };
unsigned
null_load (struct node **slot)
{
  struct node *p = *slot;
  return p ? p->value : 0;
}

/* { dg-final { scan-assembler {tst\.l a0} } } */
/* { dg-final { scan-assembler-not {move\.l \(a0\),d[0-7]} } } */
