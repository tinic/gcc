/* { dg-do compile { target *-*-amigaos* } } */
/* { dg-options "-Os -m68000 -mregparm=3 -fomit-frame-pointer" } */

unsigned long __attribute__((saveallregs))
special (const unsigned short *p, unsigned long i)
{
  return p[i];
}

/* An added late scratch could require another register save.  */
/* { dg-final { scan-assembler "and\\.l #65535" } } */

