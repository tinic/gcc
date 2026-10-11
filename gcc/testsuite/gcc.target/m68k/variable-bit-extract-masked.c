/* { dg-do compile } */
/* { dg-options "-Os -m68000 -fomit-frame-pointer" } */
#define ATTR __attribute__((noinline, noclone))
ATTR unsigned long bit_boolean (unsigned long x, unsigned int n)
{ return (x & (1UL << (n & 31))) != 0; }
ATTR unsigned long bit_extract (unsigned long x, unsigned int n)
{ return (x >> (n & 31)) & 1; }
ATTR unsigned long bit_live_count (unsigned long x, unsigned int n)
{ return ((x >> (n & 31)) & 1) ^ n; }
ATTR unsigned long bit_same_input (unsigned long x)
{ return (x >> (x & 31)) & 1; }
/* { dg-final { scan-assembler-times {btst d[0-7],d[0-7]} 4 } } */
/* { dg-final { scan-assembler-times {sne d[0-7]} 4 } } */
/* { dg-final { scan-assembler-not {lsr\.l} } } */
/* { dg-final { scan-assembler-not {and\.l #31} } } */
/* { dg-final { scan-assembler-not {moveq #31} } } */
