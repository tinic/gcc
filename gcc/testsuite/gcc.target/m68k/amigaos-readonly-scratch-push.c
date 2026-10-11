/* As amigaos-readonly-scratch.c, for the peephole2 that turns a stack pop
   followed by a constant push into moveq and a store.  C (D3) is only read,
   so it is not saved and must not be that scratch.  */
/* { dg-do compile { target *-*-amigaos* } } */
/* { dg-options "-m68000 -Os -fno-defer-pop -mregparm=0" } */

extern void g3 (int, int, int);
extern void g1 (int);
extern void fail (void) __attribute__ ((noreturn));
extern int sink;

__attribute__ ((regparm (4))) void
f (int x, int a, int b, int c)
{
  sink = c;
  if (x)
    {
      g3 (1, 2, 3);
      g1 (8);
      g3 (4, 5, 6);
      g1 (9);
      fail ();
    }
  g1 (c);
}

/* { dg-final { scan-assembler-not {,d3\n} } } */
