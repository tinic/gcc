/* A call-saved register that is only read is not saved.  Passes after reload
   must not take it as a free register: on a path that ends in a noreturn call
   it is not live at the exit, and peephole2 used it as the moveq scratch.
   An exception from that path then returned a clobbered D3 to the caller.  */
/* { dg-do compile { target *-*-amigaos* } } */
/* { dg-options "-m68000 -Os -fno-defer-pop -mregparm=4 -fexceptions" } */

extern void g (int, ...);
extern void fail (void) __attribute__ ((noreturn));
extern int sink;

void
f (int x, int a, int b, int c)
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

/* Match only writes of D3: the last operand of an instruction that
   writes its destination, a one-operand write, or an exchange.  A
   comparison or other read of D3 does not match.  */
/* { dg-final { scan-assembler-not {\n\t(?!cmp|tst|btst|chk|tas)[a-z0-9.]+[ \t][^\n]*[,:]d3(:d[0-7])?\n} } } */
/* { dg-final { scan-assembler-not {\n\t(clr|neg|negx|not|ext|extb|swap|s[a-z]+)(\.[bwl])?[ \t]d3\n} } } */
/* { dg-final { scan-assembler-not {\n\texg[ \t][^\n]*\md3\M} } } */
