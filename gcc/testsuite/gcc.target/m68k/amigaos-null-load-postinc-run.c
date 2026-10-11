/* { dg-do run } */
/* { dg-options "-O2 -m68000 -mregparm=3 -fomit-frame-pointer -fno-optimize-sibling-calls -save-temps" } */

/* The pointer load uses a postincrement source, (aN)+.  The copy through a
   free data register must keep that increment.  In null_postinc_saved the
   source walks a callee-saved address register across a call, and the
   scratch must still be call-clobbered.  */
struct node { struct node *next; unsigned value; };
extern void abort (void) __attribute__ ((stkparm));

__attribute__ ((noinline, noclone)) unsigned
null_postinc (struct node **slot)
{
  unsigned total = 0;
  struct node *p;
  while ((p = *slot++) != 0)
    total += p->value;
  return total;
}

__attribute__ ((noinline, noclone)) unsigned
visit (struct node *p)
{
  asm volatile ("" : "+a" (p));
  return p->value ^ 0x68a5d202u;
}

__attribute__ ((noinline, noclone)) unsigned
null_postinc_saved (struct node **slot)
{
  unsigned total = 0;
  struct node *p;
  while ((p = *slot++) != 0)
    total += visit (p) + p->value;
  return total;
}

int
main (void)
{
  struct node nodes[8];
  struct node *slots[9];
  unsigned seed = 0x680011;
  for (unsigned n = 0; n != 9; ++n)
    {
      unsigned plain = 0, saved = 0;
      for (unsigned i = 0; i != 8; ++i)
        {
          seed = seed * 1664525u + 1013904223u;
          nodes[i].value = seed;
          nodes[i].next = 0;
          slots[i] = &nodes[i];
        }
      /* N live entries, then the terminator.  */
      slots[n] = 0;
      for (unsigned i = 0; i != n; ++i)
        {
          plain += nodes[i].value;
          saved += (nodes[i].value ^ 0x68a5d202u) + nodes[i].value;
        }
      if (null_postinc (slots) != plain
          || null_postinc_saved (slots) != saved)
        abort ();
    }
  return 0;
}

/* Each loop load is MOVE (aN)+,Dn then MOVEA Dn,Am, and the branch uses
   the MOVE flags.  */
/* { dg-final { scan-assembler-times {move\.l \(a([0-6])\)\+,(d[01])\n\tmove\.l \2,a[0-6]\n\tj(eq|ne) } 2 } } */
/* The callee-saved source walks with a call-clobbered scratch.  */
/* { dg-final { scan-assembler-times {move\.l \(a[2-6]\)\+,d[01]\n} 1 } } */
/* { dg-final { scan-assembler-not {cmp\.w #0,a[0-6]} } } */
