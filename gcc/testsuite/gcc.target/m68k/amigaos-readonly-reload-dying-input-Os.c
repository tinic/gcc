/* { dg-do compile { target *-*-amigaos* } } */
/* { dg-options "-Os -m68020 -mregparm=4 -mno-lra" } */

/* RESULT arrives in A2 and is only read until the return value is formed.
   That LEA needs an address register; legacy reload takes A2, where RESULT
   dies, as its output reload register and then copies A2 to D0.  This used
   to fail verify_initial_elim_offsets after reload_as_needed.  From
   gcc.c-torture/compile/trunctfdf.c.  */

double *
f (long double *first, long double *last, double *result)
{
  int n;
  for (n = last - first; n > 0; --n)
    *result++ = *first++;
  return result;
}
