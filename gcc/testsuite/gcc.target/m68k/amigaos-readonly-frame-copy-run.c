/* { dg-do run { target *-*-amigaos* } } */
/* { dg-options "-O2 -m68020 -mno-lra -fomit-frame-pointer" } */

/* A local array address allocated to A5 is set from the eliminable frame
   pointer.  A5 is written, so it must be saved and restored for the caller.  */

extern void abort (void);

__attribute__ ((noipa)) void
f (unsigned int *x)
{
  unsigned char i = 0x10;
  int j;
  for (j = 0; j < 0x10; j++)
    {
      i += 0xe8;
      x[i] = 0;
      i -= 0xe7;
    }
}

__attribute__ ((noipa)) int
probe (void)
{
  unsigned int x[256];
  int i;

  for (i = 0; i < 256; i++)
    x[i] = 1;
  f (x);
  for (i = 0; i < 256; i++)
    if (x[i] != (i >= 0x08 && i < 0xf8))
      abort ();
  return 0;
}

int probe_result;

/* Call probe with a known value in A5 and return A5 afterwards.  */
__attribute__ ((noipa)) unsigned long
call_probe (void)
{
  unsigned long a5;
  __asm__ volatile ("move.l a5,-(sp)\n\t"
		    "move.l #0x5a5a1234,a5\n\t"
		    "jsr _probe\n\t"
		    "move.l d0,_probe_result\n\t"
		    "move.l a5,%0\n\t"
		    "move.l (sp)+,a5"
		    : "=d" (a5) : : "d0", "d1", "a0", "a1", "memory", "cc");
  return a5;
}

int
main (void)
{
  if (call_probe () != 0x5a5a1234 || probe_result != 0)
    abort ();
  return 0;
}
