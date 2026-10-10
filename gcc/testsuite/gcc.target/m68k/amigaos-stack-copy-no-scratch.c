/* { dg-do compile } */
/* { dg-options "-O2 -m68000 -mregparm=3 -fomit-frame-pointer -fno-optimize-sibling-calls -ffixed-a1 -ffixed-a2 -ffixed-a3 -ffixed-a4 -ffixed-a5 -ffixed-a6" } */

/* The only available address register is the live source pointer.  */
struct header { unsigned words[5]; };
extern unsigned consume (const struct header *);
unsigned
stack_copy (const struct header *src)
{
  struct header copy = *src;
  return consume (&copy);
}

/* { dg-final { scan-assembler-times {move\.l \(a0\)\+,[-0-9]+\(sp\)} 3 } } */
/* { dg-final { scan-assembler-not {move\.l \(a[0-6]\)\+,\(a[0-6]\)\+} } } */
