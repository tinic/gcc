/* { dg-do run { target *-*-amigaos* } } */
/* { dg-options "-O2 -m68000 -mregparm=3 -fomit-frame-pointer -fno-optimize-sibling-calls" } */

/* Relaxation must retain absolute calls for an out-of-range target
   and for a weak definition even when it is in the same section.  */
asm (".text\n"
     ".globl _far_target\n"
     "_far_target:\n"
     "moveq #7,d0\n"
     "rts\n"
     ".space 40000\n");
extern int far_target (void);
extern void abort (void);

__attribute__ ((weak, noinline, noclone)) int weak_target (void)
{
  asm volatile ("" : : : "memory");
  return 11;
}

int main (void)
{
  if (far_target () != 7) abort ();
  if (weak_target () != 11) abort ();
  return 0;
}
