/* { dg-do run { target *-*-amigaos* } } */
/* { dg-options "-O2 -m68000 -mregparm=3 -fomit-frame-pointer -fno-optimize-sibling-calls" } */

/* Direct calls with and without a result and an indirect call must
   preserve the return address, argument registers and stack.  */
__attribute__ ((noinline, noclone)) static unsigned
local_value (unsigned x)
{
  asm volatile ("" : "+d" (x));
  return (x << 1) + 7;
}

__attribute__ ((noinline, noclone)) static void
local_store (unsigned *p, unsigned x)
{
  asm volatile ("" : : "a" (p) : "memory");
  *p = x ^ 0x68a5d202u;
}

unsigned call_value (unsigned x)
{
  return local_value (x) ^ 0x12345678u;
}

unsigned call_void (unsigned *p, unsigned x)
{
  local_store (p, x);
  return *p;
}

unsigned call_indirect (unsigned (*fn) (unsigned), unsigned x)
{
  return fn (x) ^ 0x12345678u;
}

__attribute__ ((noinline, noclone)) unsigned external_value (unsigned x)
{
  asm volatile ("" : "+d" (x));
  return x + 0x10203040u;
}
unsigned call_external (unsigned x)
{
  return external_value (x) ^ 0x12345678u;
}

extern void abort (void);
int main (void)
{
  unsigned values[] = { 0, 1, 0x7fffffffu, 0x80000000u, 0xffffffffu,
                        0x12345678u, 0xffff0000u, 0x0000ffffu };
  unsigned i, out;
  for (i = 0; i < sizeof values / sizeof values[0]; ++i)
    {
      unsigned x = values[i];
      if (call_value (x) != (((x << 1) + 7) ^ 0x12345678u)) abort ();
      if (call_void (&out, x) != (x ^ 0x68a5d202u)) abort ();
      if (out != (x ^ 0x68a5d202u)) abort ();
      if (call_indirect (local_value, x)
          != (((x << 1) + 7) ^ 0x12345678u)) abort ();
      if (call_external (x) != ((x + 0x10203040u) ^ 0x12345678u)) abort ();
    }
  return 0;
}
