/* { dg-do compile { target *-*-amigaos* } } */
/* { dg-options "-Os -m68000 -mregparm=3 -fomit-frame-pointer" } */

unsigned long equal8 (unsigned char x __asm("d1"), unsigned char y __asm("d2"))
{ return x == y; }
unsigned long equal16 (unsigned short x __asm("d1"), unsigned short y __asm("d2"))
{ return x == y; }
unsigned long equal32 (unsigned long x __asm("d1"), unsigned long y __asm("d2"))
{ return x == y; }
unsigned long equal_mem (const volatile unsigned char *p,
                         unsigned char y __asm("d1"))
{ return *p == y; }

/* { dg-final { scan-assembler-times "neg\\.b d0" 4 } } */
/* { dg-final { scan-assembler-not "ext\\.l d0" } } */

