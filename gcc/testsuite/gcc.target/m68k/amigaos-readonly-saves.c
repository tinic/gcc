/* Reading a callee-saved register does not require a save/restore pair.  */
/* { dg-do compile } */
/* { dg-skip-if "Amiga register parameters" { ! { m68k-*-amigaos* } } } */
/* { dg-options "-m68000 -Os -mregparm=0 -fomit-frame-pointer -fno-optimize-sibling-calls" } */

typedef unsigned long word;
word read_d2 (word x __asm ("d2")) { return x + 1; }
word read_a2 (const word *p __asm ("a2")) { return p[1] + 1; }
word stack_arg (word x __asm ("d2"), word y) { return x + y; }
extern word other (word);
word read_a6_call (const volatile word *p __asm ("a6"), word x)
{ return other (x) + p[1]; }

/* { dg-final { scan-assembler-not {move\.l[ \t]+[da][26],-\(sp\)} } } */
/* { dg-final { scan-assembler-not {movem\.l[^\n]*a6} } } */
