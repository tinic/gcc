/* { dg-do compile { target *-*-amigaos* } } */
/* { dg-options "-Os -m68000 -mregparm=3 -fomit-frame-pointer -fno-optimize-sibling-calls" } */

extern unsigned long byte_sink (unsigned char x __asm("d0"));
unsigned long byte_call (const volatile unsigned long *p)
{
  return byte_sink (*p);
}

/* { dg-final { scan-assembler "move\\.l \\(a0\\),d0" } } */
/* { dg-final { scan-assembler-not "move\\.[bw] .*\\(a0\\)" } } */

