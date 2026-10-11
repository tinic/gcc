/* { dg-do compile { target *-*-amigaos* } } */
/* { dg-options "-Os -m68000 -mregparm=3 -fomit-frame-pointer" } */

unsigned long busy (const unsigned short *p, unsigned long i,
                    unsigned long x __asm("d1"))
{
  unsigned long y = p[i];
  __asm volatile ("" : "+d" (x) : "d" (y));
  return y + x;
}

/* D1 is live across the load; adding an unsaved callee register is forbidden. */
/* { dg-final { scan-assembler "and\\.l #65535" } } */

