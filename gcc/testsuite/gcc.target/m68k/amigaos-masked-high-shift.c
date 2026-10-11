/* { dg-do compile { target *-*-amigaos* } } */
/* { dg-options "-Os -m68000 -mregparm=3 -fomit-frame-pointer" } */

unsigned long shift16 (unsigned long x) { return (x >> 16) & 7; }
unsigned long shift17 (unsigned long x) { return (x >> 17) & 31; }
unsigned long shift24 (unsigned long x) { return (x >> 24) & 31; }

/* The full-width mask clears the upper half after SWAP.  */
/* { dg-final { scan-assembler-times "swap d0" 3 } } */
/* { dg-final { scan-assembler-not "clr\\.w d0" } } */

