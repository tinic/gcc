/* { dg-do compile { target *-*-amigaos* } } */
/* { dg-options "-Os -m68000 -mregparm=3 -fomit-frame-pointer -fno-optimize-sibling-calls" } */

extern unsigned long byte_sink (unsigned char x __asm("d0"));
extern unsigned long word_sink (unsigned short x __asm("d0"));

unsigned long byte_call (const unsigned long *p) { return byte_sink (*p); }
unsigned long word_call (const unsigned long *p) { return word_sink (*p); }

/* The low part of a big-endian long is at byte offsets 3 and 2.  */
/* { dg-final { scan-assembler "move\\.b 3\\(a0\\),d0" } } */
/* { dg-final { scan-assembler "move\\.w 2\\(a0\\),d0" } } */
/* { dg-final { scan-assembler-not "moveq #0" } } */

