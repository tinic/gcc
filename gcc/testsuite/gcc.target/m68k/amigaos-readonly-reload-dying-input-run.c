/* { dg-do run { target *-*-amigaos* } } */
/* { dg-options "-O2 -m68020 -mno-lra" } */

/* B arrives in A2.  Legacy reload uses A2 as a reload register where B
   dies, so A2 is written and must be saved and restored for the caller.  */

extern void abort (void);

unsigned char buf[16];

__attribute__ ((noipa, regparm (4))) void
f (unsigned char *a, unsigned char *begalt, unsigned char *b)
{
  unsigned char *pfrom = b;
  unsigned char *pto = b + 3;
  while (pfrom != begalt)
    *--pto = *--pfrom;
}

__attribute__ ((noipa)) int
call (unsigned char *p)
{
  register unsigned char *v __asm ("a2") = p;
  __asm__ volatile ("" : "+a" (v));
  f (0, buf, v);
  __asm__ volatile ("" : "+a" (v));
  return v == p;
}

int
main (void)
{
  int i;
  for (i = 0; i < 8; i++)
    buf[i] = i;
  if (!call (buf + 8))
    abort ();
  for (i = 0; i < 8; i++)
    if (buf[i + 3] != i)
      abort ();
  return 0;
}
