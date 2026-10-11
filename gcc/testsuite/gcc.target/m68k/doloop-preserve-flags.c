/* { dg-do compile } */
/* { dg-options "-Os -m68000 -fomit-frame-pointer" } */
#define ATTR __attribute__((noinline, noclone))
ATTR int fill_zero (volatile unsigned long *p, unsigned long n, unsigned long value)
{
  n = (n & 511) + 1;
  do { *p++ = value; } while (--n);
  return value == 0;
}
ATTR int sum_zero (const volatile unsigned long *p, unsigned long n)
{
  n = (n & 511) + 1;
  unsigned long sum = 0;
  do { sum += *p++; } while (--n);
  return sum == 0;
}
ATTR int sum_positive (const volatile unsigned long *p, unsigned long n)
{
  n = (n & 511) + 1;
  unsigned long sum = 0;
  do { sum += *p++; } while (--n);
  return (long) sum > 0;
}
/* The two zero tests can use preserved flags.  Signed GT after addition
   must still clear overflow with TST; preserving flags does not make a
   NOOV arithmetic result suitable for signed GT/LE.  */
/* { dg-final { scan-assembler "dbra" } } */
/* { dg-final { scan-assembler-times "tst\\.l" 1 } } */
