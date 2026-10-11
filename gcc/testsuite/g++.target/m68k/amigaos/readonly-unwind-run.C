/* A call-saved register that a function only reads is not saved, and its FDE
   says the register is unchanged.  On a path that ends in a noreturn call,
   peephole2 used such a register as a moveq scratch.  The throw below then
   unwound to the caller with the register clobbered.  F1 takes the register
   from regparm (4), F2 from an __asm register parameter; both use the moveq
   peephole for a constant store.  F3 uses the one for a pop and a push.  */
/* { dg-do run { target *-*-amigaos* } } */
/* { dg-options "-Os -m68000 -fno-defer-pop -fexceptions" } */

extern "C" void abort (void);

int sink;

__attribute__ ((noipa)) void
g (int x)
{
  sink += x;
}

__attribute__ ((noipa)) void
g3 (int x, int y, int z)
{
  sink += x + y + z;
}

__attribute__ ((noipa, noreturn)) void
fail (void)
{
  throw 1;
}

__attribute__ ((noipa, regparm (4))) void
f1 (int x, int a, int b, int c)
{
  sink = c;
  if (x)
    {
      g (5);
      g (6);
      g (7);
      g (8);
      g (9);
      fail ();
    }
  g (c);
}

__attribute__ ((noipa)) void
f2 (int a __asm ("d2"), int x)
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

__attribute__ ((noipa, regparm (4))) void
f3 (int x, int a, int b, int c)
{
  sink = c;
  if (x)
    {
      g3 (1, 2, 3);
      g (8);
      g3 (4, 5, 6);
      g (9);
      fail ();
    }
  g (c);
}

#define MAGIC 0x5a3c1234

__attribute__ ((noipa)) int
call1 (int x)
{
  register int v __asm ("d3") = MAGIC;
  __asm__ volatile ("" : "+d" (v));
  try
    {
      f1 (x, 0, 0, v);
    }
  catch (int)
    {
    }
  __asm__ volatile ("" : "+d" (v));
  return v;
}

__attribute__ ((noipa)) int
call2 (int x)
{
  register int v __asm ("d2") = MAGIC;
  __asm__ volatile ("" : "+d" (v));
  try
    {
      f2 (v, x);
    }
  catch (int)
    {
    }
  __asm__ volatile ("" : "+d" (v));
  return v;
}

__attribute__ ((noipa)) int
call3 (int x)
{
  register int v __asm ("d3") = MAGIC;
  __asm__ volatile ("" : "+d" (v));
  try
    {
      f3 (x, 0, 0, v);
    }
  catch (int)
    {
    }
  __asm__ volatile ("" : "+d" (v));
  return v;
}

int
main (void)
{
  if (call1 (0) != MAGIC || call1 (1) != MAGIC)
    abort ();
  if (call2 (0) != MAGIC || call2 (1) != MAGIC)
    abort ();
  if (call3 (0) != MAGIC || call3 (1) != MAGIC)
    abort ();
  return 0;
}
