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

/* Match only writes of D2: the last operand of an instruction that
   writes its destination, a one-operand write, or an exchange.  A
   comparison or other read of D2 does not match.  */
/* { dg-final { scan-assembler-not {\n\t(?!cmp|tst|btst|chk|tas)[a-z0-9.]+[ \t][^\n]*[,:]d2(:d[0-7])?\n} } } */
/* { dg-final { scan-assembler-not {\n\t(clr|neg|negx|not|ext|extb|swap|s[a-z]+)(\.[bwl])?[ \t]d2\n} } } */
/* { dg-final { scan-assembler-not {\n\texg[ \t][^\n]*\md2\M} } } */
