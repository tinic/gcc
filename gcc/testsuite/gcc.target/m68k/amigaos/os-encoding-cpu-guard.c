/* { dg-do compile } */
/* { dg-options "-m68020-60 -Os -mregparm=3" } */
unsigned long keep_long_constant (void) { return 256; }
/* { dg-final { scan-assembler {move\.l #256,d0} } } */
/* { dg-final { scan-assembler-not {lsl\.l} } } */
