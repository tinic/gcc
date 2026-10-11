/* As amigaos-readonly-scratch.c, with the read-only register given by an
   __asm register parameter.  D2 is not saved, so it must not be written.  */
/* { dg-do compile { target *-*-amigaos* } } */
/* { dg-options "-m68000 -Os -fno-defer-pop -mregparm=0 -fexceptions" } */

extern void g (int);
extern void fail (void) __attribute__ ((noreturn));
extern int sink;

void
f (int a __asm ("d2"), int x)
{
  sink = a;
  if (x)
    {
      g (5);
      g (6);
      g (7);
      g (8);
      g (9);
      fail ();
    }
  g (a);
}

/* { dg-final { scan-assembler-not {,d2\n} } } */
