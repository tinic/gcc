/* { dg-do run } */
/* { dg-options "-O2 -m68000 -mregparm=3 -fomit-frame-pointer -fno-optimize-sibling-calls" } */

extern void abort (void) __attribute__ ((stkparm));

__attribute__ ((noinline, noclone)) unsigned
side (unsigned value)
{
  asm volatile ("" : "+d" (value));
  return value ^ 0x5a68ff01u;
}

__attribute__ ((noinline, noclone)) unsigned
wrap_negative (unsigned x, unsigned y)
{
  if ((int) (x - y) < 0)
    return side (x);
  return x + y;
}

__attribute__ ((noinline, noclone)) unsigned
wrap_positive (unsigned x, unsigned y)
{
  if ((int) (x - y) >= 0)
    return side (y);
  return x ^ y;
}

__attribute__ ((noinline, noclone)) unsigned
wrap_pointer (unsigned x, void *y)
{
  if ((int) (x - (unsigned) y) < 0)
    return side (x);
  return x + (unsigned) y;
}

__attribute__ ((noinline, noclone)) unsigned
wrap_reused (unsigned x, unsigned y)
{
  unsigned delta = x - y;
  if ((int) delta < 0)
    return side (delta);
  return delta;
}

/* The expected branch direction is supplied as a literal, so the oracle
   cannot be compiled using the same subtraction/sign-test pattern.  */
__attribute__ ((noinline, noclone)) void
check (unsigned x, unsigned y, unsigned negative)
{
  unsigned transformed_x = x ^ 0x5a68ff01u;
  unsigned transformed_y = y ^ 0x5a68ff01u;
  unsigned delta = x - y;
  if (wrap_negative (x, y) != (negative ? transformed_x : x + y)
      || wrap_positive (x, y) != (negative ? x ^ y : transformed_y)
      || wrap_pointer (x, (void *) y) != (negative ? transformed_x : x + y)
      || wrap_reused (x, y) != (negative ? delta ^ 0x5a68ff01u : delta))
    abort ();
}

int
main (void)
{
  check (0, 0, 0);
  check (1, 0, 0);
  check (0, 1, 1);
  check (0x7fffffffu, 0xffffffffu, 1);
  check (0x80000000u, 1, 0);
  check (0xffffffffu, 0x7fffffffu, 1);
  check (0, 0x80000000u, 1);
  check (0x80000000u, 0, 1);
  check (0x7fffffffu, 0x80000000u, 1);
  check (0x80000000u, 0x7fffffffu, 0);
  check (0xffffffffu, 0xffffffffu, 0);
  check (0xffffffffu, 0, 1);
  check (0, 0xffffffffu, 0);
  return 0;
}
