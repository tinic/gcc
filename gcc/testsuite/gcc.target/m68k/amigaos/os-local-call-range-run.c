/* { dg-do run } */
/* { dg-additional-options "-fno-toplevel-reorder" } */
/* Both directions exceed a 16-bit branch displacement in the non-LTO
   layout.  Do not force an unrepresentable PC-relative call relocation.  */
static __attribute__ ((noipa)) unsigned long backward (unsigned long x)
{ return x + 3; }
static unsigned long forward (unsigned long);
__attribute__ ((noipa)) unsigned long call_forward (unsigned long x)
{ return forward (x) + 7; }
static __attribute__ ((noipa, used)) void padding (void)
{ __asm__ volatile (".space 40000"); }
static __attribute__ ((noipa)) unsigned long forward (unsigned long x)
{ return x ^ 0xabc; }
__attribute__ ((noipa)) unsigned long call_backward (unsigned long x)
{ return backward (x) + 11; }
int main (void)
{
  if (call_backward (17) != 31 || call_forward (17) != ((17 ^ 0xabc) + 7))
    __builtin_abort ();
  return 0;
}
