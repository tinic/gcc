/* { dg-do compile { target *-*-amigaos* } } */
/* { dg-options "-Os -m68000 -mregparm=3 -fomit-frame-pointer" } */

unsigned long unmasked (unsigned long x) { return x >> 16; }
unsigned long shared (unsigned long x, volatile unsigned long *p)
{
  x >>= 17;
  *p = x;
  return x & 31;
}

/* Neither an unmasked return nor an intervening full-width store can
   discard the upper-half clearing.  */
/* { dg-final { scan-assembler-times "clr\\.w d0" 2 } } */

