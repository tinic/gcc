/* { dg-do compile } */
/* { dg-options "-m68000 -O2 -mregparm=3" } */
/* Shifted MOVEQ constants are a size choice, not a timing change.  */
unsigned long keep_long_constant (void) { return 256; }
/* { dg-final { scan-assembler {move\.l #256,d0} } } */
/* { dg-final { scan-assembler-not {lsl\.l} } } */
