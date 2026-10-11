/* { dg-do compile { target *-*-amigaos* } } */
/* { dg-options "-O1 -m68000 -mno-lra" } */

/* The address of TABLE can be allocated to A5.  Its set from the eliminable
   frame pointer is not a self copy: elimination makes it an SP-based address
   and writes A5.  This used to fail verify_initial_elim_offsets after
   reload_as_needed.  Reduced from gcc.c-torture/compile/pr43415.c.  */

int
main (void)
{
  unsigned long long table[256];
  unsigned int i;
  for (i = 0; i < 256; ++i)
    {
      unsigned long long j;
      unsigned char x = i;
      for (j = 0; j < 5; ++j)
	{
	  x += x << 1;
	  x ^= x >> 1;
	}
      for (j = 0; j < 5; ++j)
	{
	  x += x << 1;
	  x ^= x >> 1;
	}
      table[i] ^= ((unsigned long long) x) << 16;
    }
  for (i = 0; i < 256; ++i)
    if ((table[i] & 0xff) == i)
      return 1;
  return 0;
}
