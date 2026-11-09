/* { dg-do compile } */
/* { dg-options "-Os -m68000" } */

volatile int sink;

void
bounded_loop (void)
{
  for (int i = 0; i < 64; ++i)
    sink = 1;
}

void
unbounded_loop (int n)
{
  for (int i = 0; i < n; ++i)
    sink = 1;
}

/* { dg-final { scan-assembler-times {\tdbra\t|\tdbra } 1 } } */
/* { dg-final { scan-assembler-not {clr\.w d[0-7];subq\.l #1} } } */
