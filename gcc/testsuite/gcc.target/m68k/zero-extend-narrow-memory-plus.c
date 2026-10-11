/* { dg-do compile } */
/* { dg-options "-Os -m68000 -fomit-frame-pointer" } */
#define ATTR __attribute__((noinline, noclone))
ATTR unsigned long byte_plus (const volatile unsigned char *p)
{ return (unsigned char) (*p + 1); }
ATTR unsigned long byte_minus (const volatile unsigned char *p)
{ return (unsigned char) (*p - 1); }
ATTR unsigned long byte_large (const volatile unsigned char *p)
{ return (unsigned char) (*p + 173); }
ATTR unsigned long word_plus (const volatile unsigned short *p)
{ return (unsigned short) (*p + 1); }
ATTR unsigned long word_minus (const volatile unsigned short *p)
{ return (unsigned short) (*p - 1); }
ATTR unsigned long word_large (const volatile unsigned short *p)
{ return (unsigned short) (*p + 33059); }
/* Each sum wraps in the original width, then clears all high SI bits.
   A single narrow read and narrow ALU replace the final full-width mask. */
/* { dg-final { scan-assembler-times "moveq #0" 6 } } */
/* { dg-final { scan-assembler-not "and\\.l #(255|65535)" } } */
