/* { dg-do compile { target *-*-amigaos* } } */
/* { dg-options "-O2 -m68000 -mregparm=3 -fomit-frame-pointer" } */

void store16 (unsigned long x, unsigned char *p) { *p = x >> 16; }
void store17 (unsigned long x, unsigned char *p) { *p = x >> 17; }
void store24 (unsigned long x, unsigned char *p) { *p = x >> 24; }
void volatile24 (unsigned long x, volatile unsigned char *p) { *p = x >> 24; }

/* { dg-final { scan-assembler-times "swap" 4 } } */
/* { dg-final { scan-assembler-not "clr\\.w" } } */

