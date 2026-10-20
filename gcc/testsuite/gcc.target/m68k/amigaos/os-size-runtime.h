/* Shared by the default and regparm (3) runtime tests.  Keep the tested
   operations out of the oracle, including under LTO.  All signed products
   below fit int32_t; the full-width unsigned products wrap modulo 2^32.  */
#include <stdint.h>
#include "os-size-runtime-vectors.h"

TEST_FN uint32_t
math_op (uint32_t x, uint32_t y, unsigned op)
{
  switch (op)
    {
    case 0: return (uint32_t) (uint16_t) x * 92;
    case 1: return (uint32_t) (uint16_t) x * 65535;
    case 2: return (uint32_t) (uint16_t) x * 3;
    case 3: return (uint32_t) (uint16_t) x * 32767;
    case 4: return (int32_t) (int16_t) x * 92;
    case 5: return (int32_t) (int16_t) x * -32768;
    case 6: return (int32_t) (int16_t) x * -3;
    case 7: return (int32_t) (int16_t) x * 32767;
    case 8: return (uint32_t) (uint16_t) x * (uint16_t) y;
    case 9: return (int32_t) (int16_t) x * (int16_t) y;
    case 10: return x * 92;
    case 11: return x * 65536;
    default: __builtin_abort ();
    }
}

static volatile uint32_t loop_observed;

TEST_FN uint32_t
loop_count (uint32_t n)
{
  uint32_t sum = 0;
  while (n--)
    {
      sum++;
      loop_observed = sum;
    }
  return sum;
}

TEST_FN uint32_t
loop_mix (uint32_t n)
{
  uint32_t sum = 0;
  while (n--)
    {
      sum += n;
      loop_observed = sum;
    }
  return sum;
}

/* Real calls keep the switch as an indexed dispatch rather than a table
   of constant results.  */
#define CALLBACK(N) \
  TEST_FN int f##N (void *p) { return (uintptr_t) p + N; }
CALLBACK (1)
CALLBACK (2)
CALLBACK (3)
CALLBACK (4)
CALLBACK (5)
CALLBACK (6)
#undef CALLBACK

TEST_FN int
dispatch (uint16_t n, void *p)
{
  switch (n)
    {
    case 1: return f1 (p);
    case 2: return f2 (p);
    case 3: return f3 (p);
    case 4: return f4 (p);
    case 5: return f5 (p);
    case 6: return f6 (p);
    default: return -1;
    }
}

TEST_FN int
dispatch_high (uint16_t n, void *p)
{
  switch (n)
    {
    case 65530: return f1 (p);
    case 65531: return f2 (p);
    case 65532: return f3 (p);
    case 65533: return f4 (p);
    case 65534: return f5 (p);
    case 65535: return f6 (p);
    default: return -1;
    }
}

/* Check the SUBQ carry/wrap semantics used by the replacement SI loop.  */
TEST_FN uint32_t
step (uint32_t x, uint32_t *taken)
{
  uint32_t t;
  __asm__ volatile ("subq.l #1,%0\n\tjcc 1f\n\tmoveq #0,%1\n\tbra 2f\n"
		    "1:\tmoveq #1,%1\n2:"
		    : "+d" (x), "=&d" (t) : : "cc");
  *taken = t;
  return x;
}

static void
check (uint32_t actual, uint32_t expected)
{
  if (actual != expected)
    __builtin_abort ();
}

/* Fixed sums avoid computing the expected result with the loop or
   multiplication being tested.  Include both sides of the DBRA boundary.  */
static const struct
{
  uint32_t n, sum;
} loops[] = {
  { 0, 0 }, { 1, 0 }, { 2, 1 }, { 3, 3 },
  { 127, 0x1f41 }, { 128, 0x1fc0 },
  { 255, 0x7e81 }, { 256, 0x7f80 },
  { 32767, 0x1fff4001 }, { 32768, 0x1fffc000 },
  { 65534, 0x7ffd8003 }, { 65535, 0x7ffe8001 },
  { 65536, 0x7fff8000 }, { 65537, 0x80008000 },
  { 131071, 0xfffd0001 }, { 131072, 0xffff0000 },
  { 131073, 0x10000 }
};

int
main (void)
{
  unsigned i, j;
  uint32_t expected_unsigned = 0, expected_signed = 0;

  /* Exhaust every input bit pattern for both signed and unsigned x * 92,
     and both switch ranges.  Repeated addition is an independent oracle.  */
  for (i = 0; i < 65536; i++)
    {
      uint32_t taken, value;
      if (i == 32768)
	expected_signed = (uint32_t) -3014656;
      check (math_op (i, 0, 0), expected_unsigned);
      check (math_op (i, 0, 4), expected_signed);
      expected_unsigned += 92;
      expected_signed += 92;
      check (dispatch (i, (void *) 0x1234),
	     i >= 1 && i <= 6 ? 0x1234 + i : (uint32_t) -1);
      check (dispatch_high (i, (void *) 0x1234),
	     i >= 65530 ? 0x1234 + i - 65529 : (uint32_t) -1);
      value = step (i, &taken);
      check (value, (uint32_t) i - 1);
      check (taken, i != 0);
    }

  for (i = 0; i < sizeof (vectors) / sizeof (vectors[0]); i++)
    {
      uint32_t taken, value;
      for (j = 0; j < 12; j++)
	check (math_op (vectors[i].x, vectors[i].y, j),
	       vectors[i].expected[j]);
      value = step (vectors[i].x, &taken);
      check (value, vectors[i].x - 1);
      check (taken, vectors[i].x != 0);
    }

  for (i = 0; i < sizeof (loops) / sizeof (loops[0]); i++)
    {
      check (loop_count (loops[i].n), loops[i].n);
      check (loop_mix (loops[i].n), loops[i].sum);
    }
  return 0;
}
