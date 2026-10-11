/* { dg-do compile { target *-*-amigaos* } } */
/* { dg-options "-Os -m68000 -mregparm=3 -fomit-frame-pointer -fno-optimize-sibling-calls" } */

extern unsigned long byte_sink (unsigned char x);
extern unsigned long word_sink (unsigned short x);
unsigned long byte_call (const unsigned long *p) { return byte_sink (*p); }
unsigned long word_call (const unsigned long *p) { return word_sink (*p); }

/* Normal prototype promotion is preserved.  */
/* { dg-final { scan-assembler-times "moveq #0,d0" 2 } } */

