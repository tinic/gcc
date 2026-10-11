/* { dg-do compile } */
/* { dg-options "-Os -m68000 -mregparm=3 -fomit-frame-pointer" } */

/* Both data argument registers are live across the indexed load.  Clearing
   another register before loading must not introduce a callee-register save. */
__attribute__((noinline, noclone))
unsigned long pressure_byte (const volatile unsigned char *p,
                             unsigned long index, unsigned long live)
{ return (unsigned char) (p[index] + 1) ^ live; }

__attribute__((noinline, noclone))
unsigned long pressure_word (const volatile unsigned short *p,
                             unsigned long index, unsigned long live)
{ return (unsigned short) (p[index] + 1) ^ live; }

/* { dg-final { scan-assembler-not "d[2-7]" } } */
/* { dg-final { scan-assembler-not "sp" } } */
