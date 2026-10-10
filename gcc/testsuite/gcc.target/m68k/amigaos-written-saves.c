/* Partial writes, overlapping wide writes and asm clobbers still need saves. */
/* { dg-do compile } */
/* { dg-skip-if "Amiga register parameters" { ! { m68k-*-amigaos* } } } */
/* { dg-options "-m68000 -Os -mregparm=0 -fomit-frame-pointer" } */

typedef unsigned long word;
word partial (word x __asm ("d2"))
{
  register word v __asm ("d2") = x;
  __asm__ volatile ("addq.w #1,%0" : "+d" (v));
  return v;
}
word overlap (word x)
{
  register unsigned long long pair __asm ("d2") = x;
  __asm__ volatile ("" : "+r" (pair));
  return (word) pair;
}
word clobber (word x)
{
  __asm__ volatile ("moveq #42,d4" ::: "d4");
  return x;
}
word increment (const word *p __asm ("a2"))
{
  word x;
  __asm__ volatile ("move.l (%0)+,%1" : "+a" (p), "=d" (x) : : "memory");
  return x;
}

/* { dg-final { scan-assembler {move\.l[ \t]+d2,-\(sp\)} } } */
/* { dg-final { scan-assembler {move\.l[ \t]+d3,-\(sp\)} } } */
/* { dg-final { scan-assembler {move\.l[ \t]+d4,-\(sp\)} } } */
/* { dg-final { scan-assembler {move\.l[ \t]+a2,-\(sp\)} } } */
