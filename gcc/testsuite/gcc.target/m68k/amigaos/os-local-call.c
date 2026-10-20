/* { dg-do compile } */
/* { dg-options "-m68000 -Os -mregparm=3 -fno-toplevel-reorder" } */
static __attribute__ ((noipa)) unsigned long local (unsigned long x)
{ return x ^ 0xabc; }
unsigned long call_local (unsigned long x) { return local (x) + 1; }
/* GAS can choose short BSR, word BSR or an absolute JSR as needed.  */
/* { dg-final { scan-assembler {jbsr _local} } } */
