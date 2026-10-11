/* { dg-do compile { target *-*-amigaos* } } */
/* { dg-options "-Os -m68000 -mregparm=3 -fomit-frame-pointer" } */

unsigned long overlap (unsigned long x __asm("d0"), unsigned long y __asm("d1"))
{ return x == y; }

/* Clearing D0 before the comparison would destroy the input.  */
/* { dg-final { scan-assembler-not "neg\\.b d0" } } */
/* { dg-final { scan-assembler "neg\\.l d0" } } */

