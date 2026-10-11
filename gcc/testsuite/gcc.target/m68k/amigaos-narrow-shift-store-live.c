/* { dg-do compile { target *-*-amigaos* } } */
/* { dg-options "-O2 -m68000 -mregparm=3 -fomit-frame-pointer" } */

/* All bits are still needed in the return value or destination index.  */
unsigned long live16 (unsigned long x, unsigned char *p)
{
  unsigned long t = x >> 16;
  *p = t;
  return t;
}

void indexed16 (unsigned long x, unsigned char *p)
{
  unsigned long t = x >> 16;
  p[t] = t;
}

/* { dg-final { scan-assembler-times "clr\\.w" 2 } } */

