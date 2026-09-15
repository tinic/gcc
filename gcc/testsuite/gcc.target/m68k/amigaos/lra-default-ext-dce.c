/* LRA is the default allocator on amigaos.  The old reload pass ICEs on
   this DImode code at -O1 -fext-dce (PR 127300,
   https://github.com/AmigaPorts/m68k-amigaos-gcc/issues/34): the check
   before the subtraction and the trailing call are both needed.  */
/* { dg-do compile } */
/* { dg-options "-O1 -fext-dce" } */

extern long long get (void *);
extern int err (void *);
extern void pop (void *);

int
getfield (void *L, int delta)
{
  long long res = get (L);
  if (res >= 0 ? res - delta > 2147483647 : res < delta)
    return err (L);
  res -= delta;
  pop (L);
  return (int) res;
}
