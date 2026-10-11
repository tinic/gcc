/* { dg-do compile } */
/* { dg-options "-Os -m68000 -fomit-frame-pointer" } */
#define ATTR __attribute__((noinline, noclone))
ATTR unsigned long bit_choose (unsigned long x, unsigned int n)
{ return (x & (1UL << (n & 31))) ? 123 : 456; }
ATTR void bit_branch (unsigned long x, unsigned int n, volatile unsigned long *p)
{ if (x & (1UL << (n & 31))) *p = 123; else *p = 456; }
ATTR unsigned long bit_keep_count (unsigned long x, unsigned int n,
                                   volatile unsigned long *p)
{ if (x & (1UL << (n & 31))) *p = 456; else *p = 123; return n; }

/* { dg-final { scan-assembler-times {btst d[0-7],d[0-7]} 3 } } */
/* { dg-final { scan-assembler-not {and\.l #31} } } */
/* { dg-final { scan-assembler-not {moveq #31} } } */
