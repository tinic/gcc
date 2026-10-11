/* { dg-do run } */
/* Bounded and early-exit loops exercise runtime iteration expressions
   after pointer induction-variable rewriting.  */
__attribute__((noinline, noclone))
int bounded_zero (const char *text, unsigned long size)
{
  for (unsigned long i = 0; i < size; i++)
    if (text[i] == 0)
      return 1;
  return 0;
}
int main (void)
{
  char text[257];
  for (unsigned int i = 0; i < sizeof text; i++)
    text[i] = 1;
  if (bounded_zero (text, 0) || bounded_zero (text, sizeof text))
    __builtin_abort ();
  for (unsigned int pos = 0; pos < sizeof text; pos++)
    {
      text[pos] = 0;
      if (bounded_zero (text, pos) || !bounded_zero (text, pos + 1)
          || !bounded_zero (text, sizeof text))
        __builtin_abort ();
      text[pos] = 1;
    }
  return 0;
}
