/* Runtime boundary, flags, aliasing and base-liveness coverage for the
   encoded-size batch.  noipa retains the calls even under LTO.  */
#include <stdint.h>
#include "os-encoding-runtime-vectors.h"

TEST_FN uint32_t
encoding_add_zero (uint32_t x)
{
  x += 300;
  return x == 0 ? 17 : x;
}

TEST_FN uint32_t
encoding_add_live (uint32_t x)
{
  uint32_t sum = x + 300;
  return sum == 0 ? x ^ 17 : sum;
}

TEST_FN uint32_t encoding_lt (uint32_t x, uint32_t y) { return x < y; }
TEST_FN uint32_t encoding_mask (uint32_t x, uint32_t y) { return -(x < y); }
TEST_FN uint32_t encoding_eq (uint32_t x, uint32_t y) { return x == y; }
TEST_FN uint32_t encoding_ne (uint32_t x) { return x != 0; }
TEST_FN uint32_t encoding_signed (uint32_t x, uint32_t y)
{ return (int32_t) x < (int32_t) y; }

TEST_FN char *encoding_pointer (char *p) { return p + 300; }
TEST_FN uintptr_t encoding_pointer_live (volatile char *p)
{ char *q = (char *) p + 300; return (uintptr_t) q ^ (unsigned char) *p; }

TEST_FN void
encoding_copy (uint32_t *d, const uint32_t *s)
{
  d[0] = s[0]; d[1] = s[1]; d[2] = s[2]; d[3] = s[3];
}

TEST_FN void
encoding_copy_two (uint32_t *d, const uint32_t *s)
{
  d[0] = s[0]; d[1] = s[1];
}

TEST_FN void
encoding_volatile_copy (volatile uint32_t *d, const volatile uint32_t *s)
{
  d[0] = s[0]; d[1] = s[1]; d[2] = s[2]; d[3] = s[3];
}

TEST_FN uintptr_t
encoding_copy_live (uint32_t *d, const uint32_t *s)
{
  d[0] = s[0]; d[1] = s[1]; d[2] = s[2]; d[3] = s[3];
  d[8] = (uintptr_t) s;
  return (uintptr_t) d;
}

TEST_FN void
encoding_copy_same_base (volatile uint32_t *p)
{
  p[0] = p[0]; p[1] = p[1]; p[2] = p[2]; p[3] = p[3];
}

TEST_FN uint32_t
encoding_word_loop (const uint32_t *p, uint16_t n)
{
  uint32_t sum = 0;
  while (n--)
    sum += *p++;
  return sum;
}

TEST_FN uint32_t
encoding_bytes (const volatile unsigned char *p)
{
  uint32_t a = p[0], b = p[2], c = p[4], d = p[6];
  return (a << 24) | (b << 16) | (c << 8) | d;
}

/* Force actual constant construction, not an indexed pool lookup.  */
#define CONST_FN(N, C) TEST_FN uint32_t encoding_const_##N (void) { return C; }
CONST_FN (a, 0x100U)
CONST_FN (b, 0x200U)
CONST_FN (c, 0xffffffU)
CONST_FN (d, 0x7fffffffU)
CONST_FN (e, 0x3fffffffU)
CONST_FN (f, 0xfffffe00U)
CONST_FN (g, 0x7f00U)
CONST_FN (h, 0xffffc000U)
CONST_FN (i, 0x12345678U)
#undef CONST_FN

static __attribute__ ((noipa)) uint32_t
encoding_local (uint32_t x)
{
  return x ^ 0xabc;
}

TEST_FN uint32_t encoding_call (uint32_t x) { return encoding_local (x) + 1; }

static void
encoding_check (uint32_t actual, uint32_t expected)
{
  if (actual != expected)
    __builtin_abort ();
}

/* Keep carry-dependent multiword operations intact when a nearby scalar
   constant is now synthesized by shifts (which change X).  */
TEST_FN uint64_t encoding_add64 (uint64_t x, uint64_t y) { return x + y; }
TEST_FN uint64_t encoding_sub64 (uint64_t x, uint64_t y) { return x - y; }
TEST_FN uint64_t encoding_add64_const (uint64_t x)
{ return x + 0x10000000100ULL; }
TEST_FN uint64_t encoding_sub64_const (uint64_t x)
{ return x - 0x00ffffff00ffffffULL; }

static uint32_t copy_data[32], loop_data[65536];
static char pointer_data[1024];
static volatile unsigned char byte_data[8];

int
main (void)
{
  unsigned i, s, d, k;
  encoding_check (encoding_const_a (), 0x100U);
  encoding_check (encoding_const_b (), 0x200U);
  encoding_check (encoding_const_c (), 0xffffffU);
  encoding_check (encoding_const_d (), 0x7fffffffU);
  encoding_check (encoding_const_e (), 0x3fffffffU);
  encoding_check (encoding_const_f (), 0xfffffe00U);
  encoding_check (encoding_const_g (), 0x7f00U);
  encoding_check (encoding_const_h (), 0xffffc000U);
  encoding_check (encoding_const_i (), 0x12345678U);

  for (i = 0; i < sizeof (encoding_vectors) / sizeof (encoding_vectors[0]); ++i)
    {
      uint32_t x = encoding_vectors[i].x, y = encoding_vectors[i].y;
      encoding_check (encoding_add_zero (x), encoding_vectors[i].expected[0]);
      encoding_check (encoding_add_live (x), encoding_vectors[i].expected[1]);
      encoding_check (encoding_lt (x, y), encoding_vectors[i].expected[2]);
      encoding_check (encoding_mask (x, y), encoding_vectors[i].expected[3]);
      encoding_check (encoding_eq (x, y), encoding_vectors[i].expected[4]);
      encoding_check (encoding_ne (x), encoding_vectors[i].expected[5]);
      encoding_check (encoding_signed (x, y), encoding_vectors[i].expected[6]);
      byte_data[0] = x >> 24; byte_data[2] = x >> 16;
      byte_data[4] = x >> 8; byte_data[6] = x;
      encoding_check (encoding_bytes (byte_data), x);
      encoding_check (encoding_call (x), (x ^ 0xabc) + 1);
    }
  for (i = 0; i < sizeof (wide_vectors) / sizeof (wide_vectors[0]); ++i)
    {
      uint64_t x = wide_vectors[i].x, y = wide_vectors[i].y;
      if (encoding_add64 (x, y) != wide_vectors[i].expected[0]
	  || encoding_sub64 (x, y) != wide_vectors[i].expected[1]
	  || encoding_add64_const (x) != wide_vectors[i].expected[2]
	  || encoding_sub64_const (x) != wide_vectors[i].expected[3])
	__builtin_abort ();
    }
  for (i = 0; i < 700; ++i)
    {
      pointer_data[i] = i;
      encoding_check ((uintptr_t) encoding_pointer (pointer_data + i),
		      (uintptr_t) (pointer_data + i + 300));
      encoding_check (encoding_pointer_live (pointer_data + i),
		      (uintptr_t) (pointer_data + i + 300) ^ (unsigned char) i);
    }
  for (s = 0; s < 8; ++s)
    for (d = 0; d < 8; ++d)
      {
	for (i = 0; i < 32; ++i)
	  copy_data[i] = 0xa55a0000U + i;
	encoding_copy_two (copy_data + d, copy_data + s);
	for (i = 0; i < 32; ++i)
	  encoding_check (copy_data[i], i >= d && i < d + 2
	                  ? copy_expected[s][d][i] : 0xa55a0000U + i);
	for (k = 0; k < 3; ++k)
	  {
	    for (i = 0; i < 32; ++i)
	      copy_data[i] = 0xa55a0000U + i;
	    if (k == 0)
	      encoding_copy (copy_data + d, copy_data + s);
	    else if (k == 1)
	      encoding_volatile_copy (copy_data + d, copy_data + s);
	    else
	      encoding_check (encoding_copy_live (copy_data + d, copy_data + s),
			      (uintptr_t) (copy_data + d));
	    for (i = 0; i < 32; ++i)
	      encoding_check (copy_data[i],
		k == 2 && i == d + 8 ? (uintptr_t) (copy_data + s)
				    : copy_expected[s][d][i]);
	  }
      }
  for (i = 0; i < 32; ++i)
    copy_data[i] = i;
  encoding_copy_same_base (copy_data);
  for (i = 0; i < 32; ++i)
    encoding_check (copy_data[i], i);
  for (i = 0; i < 65536; ++i)
    loop_data[i] = i;
#define LOOP(N, S) encoding_check (encoding_word_loop (loop_data, N), S)
  LOOP (0, 0); LOOP (1, 0); LOOP (2, 1); LOOP (127, 8001);
  LOOP (128, 8128); LOOP (255, 32385); LOOP (256, 32640);
  LOOP (32767, 536821761); LOOP (32768, 536854528);
  LOOP (65534, 2147319811U); LOOP (65535, 2147385345U);
#undef LOOP
  return 0;
}
