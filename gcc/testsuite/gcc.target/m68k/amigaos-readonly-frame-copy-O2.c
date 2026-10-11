/* { dg-do compile { target *-*-amigaos* } } */
/* { dg-options "-O2 -m68020 -mno-lra" } */

/* As amigaos-readonly-frame-copy.c, with the array address in A5 across a
   call.  Reduced from gcc.c-torture/execute/20030916-1.c.  */

extern void f (unsigned int *);
extern void abort (void);

int
main (void)
{
  unsigned int x[256];
  int i;

  for (i = 0; i < 256; i++)
    x[i] = 1;
  f (x);
  for (i = 0; i < 256; i++)
    if (x[i] != (i >= 0x08 && i < 0xf8))
      abort ();
  return 0;
}
