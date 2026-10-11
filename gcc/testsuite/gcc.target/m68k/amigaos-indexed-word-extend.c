/* { dg-do compile { target *-*-amigaos* } } */
/* { dg-options "-Os -m68000 -mregparm=3 -fomit-frame-pointer" } */

unsigned long word_at (const unsigned short *p, unsigned long i)
{
  return p[i];
}
unsigned long volatile_word_at (const volatile unsigned short *p,
                                unsigned long i)
{
  return p[i];
}

/* Preserve the word access and its original index, then copy the zero
   extended value from a caller-clobbered scratch.  */
/* { dg-final { scan-assembler-times "moveq #0,d1" 2 } } */
/* { dg-final { scan-assembler-times "move\\.w \\(a0,d0\\.l\\),d1" 2 } } */
/* { dg-final { scan-assembler-not "and\\.l #65535" } } */

