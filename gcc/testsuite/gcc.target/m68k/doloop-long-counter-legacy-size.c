/* { dg-do compile } */
/* { dg-options "-Os -m68000 -fomit-frame-pointer -fno-branch-count-reg" } */
#define ATTR __attribute__((noinline, noclone))
ATTR unsigned long decrement_visible (unsigned long count,
                                      volatile unsigned long *p)
{
  do { *p = count; } while (count-- != 0);
  return count;
}
ATTR long decrement_signed_visible (long count, volatile long *p)
{
  if (count < 0)
    return count;
  do { *p = count; } while (--count >= 0);
  return count;
}
/* These loops use the ordinary decrement/branch pattern and retain the
   final counter value, independently of the RTL doloop pass.  */
/* { dg-final { scan-assembler-times "subq\\.l #1" 2 } } */
/* { dg-final { scan-assembler-not "(dbra|clr\\.w)" } } */
