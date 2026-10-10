/* { dg-do compile } */
/* { dg-options "-O2 -m68000 -mregparm=3 -fomit-frame-pointer -fno-optimize-sibling-calls" } */

struct header { unsigned words[5]; };
extern unsigned consume (const struct header *);

unsigned
stack_copy (const struct header *src)
{
  struct header copy = *src;
  return consume (&copy);
}

unsigned
stack_copy_live (const struct header *src, unsigned add)
{
  struct header copy = *src;
  return consume (&copy) + add;
}

/* Each copy has four postincrement destinations and a final ordinary one. */
/* { dg-final { scan-assembler-times {move\.l \(a[0-6]\)\+,\(a[0-6]\)\+} 8 } } */
/* { dg-final { scan-assembler-not {move\.l \(a[0-6]\)\+,[-0-9]+\(sp\)} } } */
