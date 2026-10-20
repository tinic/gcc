/* { dg-do compile } */
/* { dg-options "-m68000 -Os -mregparm=3" } */
unsigned long narrow92(unsigned short x) { return (unsigned long)x * 92; }
unsigned long full92(unsigned long x) { return x * 92; }
/* The full-width operand must not be truncated to a word multiply.  */
/* { dg-final { scan-assembler-times {mulu\.w} 1 } } */
/* { dg-final { scan-assembler {mulu\.w #92} } } */
