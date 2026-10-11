/* { dg-do run } */
/* { dg-options "-O2 -m68000 -mregparm=3 -fomit-frame-pointer -fno-optimize-sibling-calls -save-temps" } */

/* A volatile stack destination filled from a postincrement source must
   still use the scratch walk, with each store kept once and in order.  */
extern void abort (void) __attribute__ ((stkparm));
struct header { unsigned words[5]; };

__attribute__ ((noinline, noclone)) unsigned
consume (const volatile struct header *p)
{
  unsigned sum = 0;
  for (unsigned i = 0; i != 5; ++i)
    sum += (i + 1) * p->words[i];
  return sum;
}

__attribute__ ((noinline, noclone)) unsigned
stack_copy_volatile (const struct header *src)
{
  volatile struct header copy;
  copy = *src;
  return consume (&copy);
}

int
main (void)
{
  struct header source;
  unsigned seed = 0x680015;
  for (unsigned n = 0; n != 128; ++n)
    {
      unsigned sum = 0;
      for (unsigned i = 0; i != 5; ++i)
        {
          seed = seed * 1664525u + 1013904223u;
          source.words[i] = seed;
          sum += (i + 1) * seed;
        }
      if (stack_copy_volatile (&source) != sum)
        abort ();
    }
  return 0;
}

/* Four postincrement stores and a final ordinary one through the scratch;
   no stack-displacement store remains.  */
/* { dg-final { scan-assembler-times {move\.l \(a[0-6]\)\+,\(a[0-6]\)\+\n} 4 } } */
/* { dg-final { scan-assembler-times {move\.l \(a[0-6]\)\+,\(a[0-6]\)\+\n\tmove\.l \(a[0-6]\),\(a[0-6]\)\n} 1 } } */
/* { dg-final { scan-assembler-not {move\.l \(a[0-6]\)\+,(-?[0-9]+)?\(sp\)} } } */
