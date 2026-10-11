/* As amigaos-readonly-scratch.c, for regrename: C (D3) is only read, so it
   is not saved, and regrename must not rename a chain on the noreturn path
   into D3.  */
/* { dg-do compile { target *-*-amigaos* } } */
/* { dg-options "-m68060 -O2 -mregparm=4 -frename-registers -fno-peephole2" } */

extern void fail (void) __attribute__ ((noreturn));
extern void g (int);
extern int sink;
extern int *ptr;
extern int out[16];

void
f (int x, int a, int b, int c)
{
  sink = c;
  if (x)
    {
      int *p = ptr;
      int t0 = p[0] * 3;
      int t1 = p[1] * 4;
      int t2 = p[2] * 5;
      int t3 = p[3] * 6;
      int t4 = p[4] * 7;
      out[0] = t0 + t1;
      out[1] = t1 + t2;
      out[2] = t2 + t3;
      out[3] = t3 + t4;
      out[4] = t4 + t0;
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
