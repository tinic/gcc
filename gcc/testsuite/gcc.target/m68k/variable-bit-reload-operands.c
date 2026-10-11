/* { dg-do compile } */
/* { dg-options "-Os -m68000 -fomit-frame-pointer" } */

#define ATTR __attribute__((noinline, noclone))
ATTR unsigned long set_masked (unsigned long x, unsigned int n)
{ return x | (1UL << (n & 31)); }
ATTR unsigned long clear_masked (unsigned long x, unsigned int n)
{ return x & ~(1UL << (n & 31)); }
ATTR unsigned long flip_masked (unsigned long x, unsigned int n)
{ return x ^ (1UL << (n & 31)); }

/* Volatile objects must retain their original long read and write. */
ATTR void set_memory_masked (volatile unsigned long *p, unsigned int n)
{ *p |= 1UL << (n & 31); }
ATTR void clear_memory_masked (volatile unsigned long *p, unsigned int n)
{ *p &= ~(1UL << (n & 31)); }
ATTR void flip_memory_masked (volatile unsigned long *p, unsigned int n)
{ *p ^= 1UL << (n & 31); }

/* { dg-final { scan-assembler-times {bset d[0-7],d[0-7]} 2 } } */
/* { dg-final { scan-assembler-times {bclr d[0-7],d[0-7]} 2 } } */
/* { dg-final { scan-assembler-times {bchg d[0-7],d[0-7]} 2 } } */
/* { dg-final { scan-assembler-not {(lsl|rol)\.l} } } */
