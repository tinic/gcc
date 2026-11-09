/* { dg-do compile } */
/* { dg-options "-Os -m68000" } */

unsigned int
clear_low_byte (unsigned int x)
{
  return (x + 1) & 0xffffff00u;
}

unsigned int
mask_low_word_from_mem (const unsigned int *p)
{
  return *p & 0xffffu;
}

/* { dg-final { scan-assembler {clr\.b d0} } } */
/* { dg-final { scan-assembler-not {and\.[lw] #} } } */
