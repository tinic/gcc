/* { dg-do run } */
/* { dg-options "-O2 -m68000 -mregparm=3 -fomit-frame-pointer -fno-optimize-sibling-calls" } */
extern void abort (void) __attribute__ ((stkparm));
struct header { unsigned words[5]; };
extern unsigned consume (const struct header *);
__attribute__ ((noinline, noclone)) unsigned stack_copy (const struct header *src)
{
  struct header copy = *src;
  return consume (&copy);
}
__attribute__ ((noinline, noclone)) unsigned stack_copy_live (const struct header *src, unsigned add)
{
  struct header copy = *src;
  return consume (&copy) + add;
}
__attribute__ ((noinline, noclone)) unsigned stack_copy_volatile (const struct header *src, unsigned add)
{
  volatile struct header copy;
  copy.words[0] = src->words[0];
  copy.words[1] = src->words[1];
  copy.words[2] = src->words[2];
  copy.words[3] = src->words[3];
  copy.words[4] = src->words[4];
  return copy.words[add % 5];
}
__attribute__ ((noinline, noclone)) unsigned stack_copy_volatile_source (const volatile struct header *src)
{
  struct header copy = *src;
  return consume (&copy);
}

__attribute__ ((noinline, noclone)) unsigned
consume (const struct header *p)
{
  unsigned sum = 0;
  asm volatile ("" : : "m" (*p) : "memory");
  for (unsigned i = 0; i != 5; ++i)
    sum += (i + 1) * p->words[i];
  return sum;
}

int
main (void)
{
  struct header source;
  unsigned seed = 0x680005;
  for (unsigned n = 0; n != 128; ++n)
    {
      unsigned sum = 0;
      for (unsigned i = 0; i != 5; ++i)
        {
          seed = seed * 1664525u + 1013904223u;
          source.words[i] = seed;
          sum += (i + 1) * seed;
        }
      if (stack_copy (&source) != sum
          || stack_copy_live (&source, seed) != sum + seed
          || stack_copy_volatile (&source, n) != source.words[n % 5]
          || stack_copy_volatile_source (&source) != sum)
        abort ();
    }
  return 0;
}
