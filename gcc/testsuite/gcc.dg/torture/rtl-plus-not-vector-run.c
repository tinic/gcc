/* { dg-do run } */
typedef unsigned int V __attribute__((vector_size(16)));
static volatile V input, output;
__attribute__((noinline, noclone))
void calculate (void)
{
  V x = input;
  output = x + ~x;
  output = ~x + x;
}
int main (void)
{
  for (unsigned int i = 0; i < 32; i++)
    {
      V x = { i, ~i, 0, 1 };
      input = x;
      calculate ();
      V y = output;
      for (unsigned int j = 0; j < sizeof (V) / sizeof (unsigned int); j++)
        if (y[j] != ~(unsigned int) 0)
          __builtin_abort ();
    }
  return 0;
}
