/* { dg-do run } */
/* { dg-options "-O2 -m68000 -mregparm=3 -fomit-frame-pointer" } */

struct node { struct node *next; unsigned value; };
extern void abort (void) __attribute__ ((stkparm));

__attribute__ ((noinline, noclone)) unsigned
null_load (struct node **slot)
{
  struct node *p = *slot;
  return p ? p->value : 0;
}

__attribute__ ((noinline, noclone)) unsigned
null_volatile (struct node * volatile *slot, unsigned fallback)
{
  struct node *p = *slot;
  return p ? p->value : fallback;
}

__attribute__ ((noinline, noclone)) unsigned
null_chain (struct node *p)
{
  unsigned total = 0;
  while ((p = p->next) != 0)
    total += p->value;
  return total;
}

__attribute__ ((noinline, noclone)) unsigned
null_pressure (struct node **slot, unsigned fallback, unsigned extra)
{
  struct node *p = *slot;
  if (!p)
    return fallback ^ extra;
  return p->value + fallback + extra;
}

int
main (void)
{
  struct node nodes[8];
  struct node *slot;
  struct node * volatile vslot;
  unsigned seed = 0x680001, total;
  for (unsigned n = 0; n != 128; ++n)
    {
      total = 0;
      for (unsigned i = 0; i != 8; ++i)
        {
          seed = seed * 1664525u + 1013904223u;
          nodes[i].value = seed;
          nodes[i].next = i == 7 ? 0 : &nodes[i + 1];
          if (i != 0)
            total += seed;
        }
      slot = 0;
      vslot = 0;
      if (null_load (&slot) != 0
          || null_volatile (&vslot, seed) != seed
          || null_pressure (&slot, seed, n) != (seed ^ n))
        abort ();
      for (unsigned i = 0; i != 8; ++i)
        {
          slot = &nodes[i];
          vslot = &nodes[i];
          if (null_load (&slot) != nodes[i].value
              || null_volatile (&vslot, seed) != nodes[i].value
              || null_pressure (&slot, seed, n) != nodes[i].value + seed + n)
            abort ();
        }
      if (null_chain (nodes) != total || null_chain (&nodes[7]) != 0)
        abort ();
    }
  return 0;
}
