/* { dg-do compile { target *-*-amigaos* } } */
/* { dg-options "-O2 -m68000 -mregparm=3 -fomit-frame-pointer -fno-optimize-sibling-calls" } */

/* Both void and value direct calls may relax.  The indirect call must
   remain JSR, and an undefined target is left for the assembler to handle.  */
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

extern unsigned external_value (unsigned);
unsigned call_external (unsigned x)
{
  return external_value (x) ^ 0x12345678u;
}

/* { dg-final { scan-assembler-times {jbsr _} 3 } } */
/* { dg-final { scan-assembler-times {jsr \(a0\)} 1 } } */
