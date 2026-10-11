/* { dg-do run } */
/* { dg-options "-Os -m68000 -fomit-frame-pointer" } */

#define ATTR __attribute__((noinline, noclone))
ATTR unsigned long arithmetic (unsigned long x, unsigned long y)
{ return x + y + 8 - 9; }
ATTR unsigned long logic (unsigned long x, unsigned long y)
{ return ((x | 127) ^ 65535) + (y & -256L); }
ATTR unsigned long bit_ops (unsigned long x, unsigned long y)
{ return ((x & 0xffff0000UL) ^ 0x80000000UL) | (y & 65535); }
ATTR unsigned long unary (unsigned long x, unsigned long y)
{ return ~x + -y; }
ATTR unsigned long materialize (unsigned long x, unsigned long y)
{ return ((x & 127) | 0x10000UL) + ((y ^ -128L) & 0x7fffffffUL); }

/* Golden results are evaluated independently on the host, not by another
   expression that could undergo the same target cost-driven rewrite.  */
static const struct {
  unsigned long x, y, arithmetic, logic, bit_ops, unary, materialize;
} cases[] = {
  { 0x00000000UL, 0x00000001UL, 0x00000000UL, 0x0000ff80UL, 0x80000001UL, 0xfffffffeUL, 0x8000ff81UL },
  { 0x00000000UL, 0xffffffffUL, 0xfffffffeUL, 0x0000fe80UL, 0x8000ffffUL, 0x00000000UL, 0x0001007fUL },
  { 0x00000001UL, 0x00000007UL, 0x00000007UL, 0x0000ff80UL, 0x80000007UL, 0xfffffff7UL, 0x8000ff88UL },
  { 0x00000001UL, 0x80000000UL, 0x80000000UL, 0x8000ff80UL, 0x80000000UL, 0x7ffffffeUL, 0x8000ff81UL },
  { 0x00000007UL, 0x00000008UL, 0x0000000eUL, 0x0000ff80UL, 0x80000008UL, 0xfffffff0UL, 0x8000ff8fUL },
  { 0x00000007UL, 0x7fffffffUL, 0x80000005UL, 0x8000fe80UL, 0x8000ffffUL, 0x7ffffff9UL, 0x00010086UL },
  { 0x00000008UL, 0x0000007fUL, 0x00000086UL, 0x0000ff80UL, 0x8000007fUL, 0xffffff78UL, 0x80010007UL },
  { 0x00000008UL, 0x00010000UL, 0x00010007UL, 0x0001ff80UL, 0x80000000UL, 0xfffefff7UL, 0x7fffff88UL },
  { 0x0000007fUL, 0x00000080UL, 0x000000feUL, 0x0000ff80UL, 0x80000080UL, 0xffffff00UL, 0x8000ff7fUL },
  { 0x0000007fUL, 0x0000ffffUL, 0x0001007dUL, 0x0001fe80UL, 0x8000ffffUL, 0xfffeff81UL, 0x800000feUL },
  { 0x00000080UL, 0x000000ffUL, 0x0000017eUL, 0x0000ff00UL, 0x800000ffUL, 0xfffffe80UL, 0x8000ff7fUL },
  { 0x00000080UL, 0x00000100UL, 0x0000017fUL, 0x00010000UL, 0x80000100UL, 0xfffffe7fUL, 0x8000fe80UL },
  { 0x000000ffUL, 0x00000100UL, 0x000001feUL, 0x00010000UL, 0x80000100UL, 0xfffffe00UL, 0x8000feffUL },
  { 0x000000ffUL, 0x000000ffUL, 0x000001fdUL, 0x0000ff00UL, 0x800000ffUL, 0xfffffe01UL, 0x8000fffeUL },
  { 0x00000100UL, 0x0000ffffUL, 0x000100feUL, 0x0001fd80UL, 0x8000ffffUL, 0xfffeff00UL, 0x8000007fUL },
  { 0x00000100UL, 0x00000080UL, 0x0000017fUL, 0x0000fe80UL, 0x80000080UL, 0xfffffe7fUL, 0x8000ff00UL },
  { 0x0000ffffUL, 0x00010000UL, 0x0001fffeUL, 0x00010000UL, 0x80000000UL, 0xfffe0000UL, 0x7fffffffUL },
  { 0x0000ffffUL, 0x0000007fUL, 0x0001007dUL, 0x00000000UL, 0x8000007fUL, 0xfffeff81UL, 0x8001007eUL },
  { 0x00010000UL, 0x7fffffffUL, 0x8000fffeUL, 0x8001fe80UL, 0x8001ffffUL, 0x7fff0000UL, 0x0001007fUL },
  { 0x00010000UL, 0x00000008UL, 0x00010007UL, 0x0001ff80UL, 0x80010008UL, 0xfffefff7UL, 0x8000ff88UL },
  { 0x7fffffffUL, 0x80000000UL, 0xfffffffeUL, 0xffff0000UL, 0xffff0000UL, 0x00000000UL, 0x8000ffffUL },
  { 0x7fffffffUL, 0x00000007UL, 0x80000005UL, 0x7fff0000UL, 0xffff0007UL, 0x7ffffff9UL, 0x80010006UL },
  { 0x80000000UL, 0xffffffffUL, 0x7ffffffeUL, 0x8000fe80UL, 0x0000ffffUL, 0x80000000UL, 0x0001007fUL },
  { 0x80000000UL, 0x00000001UL, 0x80000000UL, 0x8000ff80UL, 0x00000001UL, 0x7ffffffeUL, 0x8000ff81UL },
  { 0xffffffffUL, 0x00000000UL, 0xfffffffeUL, 0xffff0000UL, 0x7fff0000UL, 0x00000000UL, 0x8000ffffUL },
};
int main (void)
{
  for (unsigned int i = 0; i < sizeof cases / sizeof cases[0]; i++)
    if (arithmetic (cases[i].x, cases[i].y) != cases[i].arithmetic
        || logic (cases[i].x, cases[i].y) != cases[i].logic
        || bit_ops (cases[i].x, cases[i].y) != cases[i].bit_ops
        || unary (cases[i].x, cases[i].y) != cases[i].unary
        || materialize (cases[i].x, cases[i].y) != cases[i].materialize)
      __builtin_abort ();
  return 0;
}
