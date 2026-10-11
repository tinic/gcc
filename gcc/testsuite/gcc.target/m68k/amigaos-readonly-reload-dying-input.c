/* { dg-do compile { target *-*-amigaos* } } */
/* { dg-options "-O2 -m68000 -mregparm=4 -mno-lra" } */

/* B arrives in A2, a call-saved register that the function otherwise only
   reads.  Legacy reload takes A2, where B dies, as the reload register for
   the subtraction that sizes the copy, so A2 is written.  That reload
   register is chosen by find_reloads, not taken from the spill registers.
   It must count as a write before the save area is final.  This used to
   fail verify_initial_elim_offsets after reload_as_needed.  Reduced from
   gcc.c-torture/compile/20090331-1.c.  */

void
f (unsigned char *a, unsigned char *begalt, unsigned char *b)
{
  unsigned char *pfrom = b;
  unsigned char *pto = b + 3;
  while (pfrom != begalt)
    *--pto = *--pfrom;
}
