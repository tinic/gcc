/* { dg-do run } */
/* { dg-options "-Os -m68000 -fomit-frame-pointer" } */
#include "variable-bit-reload-operands.c"

/* Expected masks are independent literals, including high count bits. */
static const struct { unsigned int count; unsigned long mask; } cases[] = {
  { 0, 0x00000001UL },
  { 1, 0x00000002UL },
  { 2, 0x00000004UL },
  { 3, 0x00000008UL },
  { 4, 0x00000010UL },
  { 5, 0x00000020UL },
  { 6, 0x00000040UL },
  { 7, 0x00000080UL },
  { 8, 0x00000100UL },
  { 9, 0x00000200UL },
  { 10, 0x00000400UL },
  { 11, 0x00000800UL },
  { 12, 0x00001000UL },
  { 13, 0x00002000UL },
  { 14, 0x00004000UL },
  { 15, 0x00008000UL },
  { 16, 0x00010000UL },
  { 17, 0x00020000UL },
  { 18, 0x00040000UL },
  { 19, 0x00080000UL },
  { 20, 0x00100000UL },
  { 21, 0x00200000UL },
  { 22, 0x00400000UL },
  { 23, 0x00800000UL },
  { 24, 0x01000000UL },
  { 25, 0x02000000UL },
  { 26, 0x04000000UL },
  { 27, 0x08000000UL },
  { 28, 0x10000000UL },
  { 29, 0x20000000UL },
  { 30, 0x40000000UL },
  { 31, 0x80000000UL },
  { 32, 0x00000001UL },
  { 33, 0x00000002UL },
  { 63, 0x80000000UL },
  { 64, 0x00000001UL },
  { 65, 0x00000002UL },
  { 255, 0x80000000UL },
  { 256, 0x00000001UL },
  { 0xffffffffU, 0x80000000UL },
};
static const unsigned long values[] =
  { 0UL, 0xffffffffUL, 0xaaaaaaaaUL, 0x55555555UL, 0x80000000UL, 1UL };
static volatile struct { unsigned long before, value, after; } memory;

int main (void)
{
  for (unsigned int i = 0; i < sizeof cases / sizeof cases[0]; i++)
    for (unsigned int j = 0; j < sizeof values / sizeof values[0]; j++)
      {
        unsigned int n = cases[i].count;
        unsigned long x = values[j], mask = cases[i].mask;
        unsigned long set = x | mask, clear = x & ~mask, flip = x ^ mask;
        if (set_masked (x, n) != set || clear_masked (x, n) != clear
            || flip_masked (x, n) != flip)
          __builtin_abort ();
        memory.before = 0x12345678UL;
        memory.after = 0x89abcdefUL;
        memory.value = x;
        set_memory_masked (&memory.value, n);
        if (memory.value != set)
          __builtin_abort ();
        memory.value = x;
        clear_memory_masked (&memory.value, n);
        if (memory.value != clear)
          __builtin_abort ();
        memory.value = x;
        flip_memory_masked (&memory.value, n);
        if (memory.value != flip || memory.before != 0x12345678UL
            || memory.after != 0x89abcdefUL)
          __builtin_abort ();
      }
  return 0;
}
