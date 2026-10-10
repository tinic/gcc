/* Exercise results, stack arguments and every callee-saved integer register. */
/* { dg-do run } */
/* { dg-skip-if "Amiga register parameters" { ! { m68k-*-amigaos* } } } */
/* { dg-options "-m68000 -Os -mregparm=0 -fomit-frame-pointer -fno-optimize-sibling-calls" } */

typedef unsigned long word;
extern void abort (void) __attribute__ ((noreturn, stkparm));
extern word check_saved (void *, word, word, word, word, word, word)
  __attribute__ ((stkparm));

/* fn, D2, D3, A2, A6, stack argument, expected result.  Preserve our caller's
   register values separately from the values used to exercise the callee. */
__asm__ (
  ".text\n\t.align 2\n_check_saved:\n"
  "\tmovem.l d2-d7/a2-a6,-(sp)\n"
  "\tmove.l 48(sp),a0\n\tmove.l 52(sp),d2\n\tmove.l 56(sp),d3\n"
  "\tmove.l 60(sp),a2\n\tmove.l 64(sp),a6\n"
  "\tmove.l #0x12345674,d4\n\tmove.l #0x12345675,d5\n"
  "\tmove.l #0x12345676,d6\n\tmove.l #0x12345677,d7\n"
  "\tmove.l #0x12345673,a3\n\tmove.l #0x12345674,a4\n"
  "\tmove.l #0x12345675,a5\n\tmove.l 68(sp),-(sp)\n"
  "\tjsr (a0)\n"
  "\tcmp.l 76(sp),d0\n\tbne .Lsave_fail\n"
  "\tcmp.l 56(sp),d2\n\tbne .Lsave_fail\n"
  "\tcmp.l 60(sp),d3\n\tbne .Lsave_fail\n"
  "\tcmpa.l 64(sp),a2\n\tbne .Lsave_fail\n"
  "\tcmpa.l 68(sp),a6\n\tbne .Lsave_fail\n"
  "\tcmp.l #0x12345674,d4\n\tbne .Lsave_fail\n"
  "\tcmp.l #0x12345675,d5\n\tbne .Lsave_fail\n"
  "\tcmp.l #0x12345676,d6\n\tbne .Lsave_fail\n"
  "\tcmp.l #0x12345677,d7\n\tbne .Lsave_fail\n"
  "\tcmpa.l #0x12345673,a3\n\tbne .Lsave_fail\n"
  "\tcmpa.l #0x12345674,a4\n\tbne .Lsave_fail\n"
  "\tcmpa.l #0x12345675,a5\n\tbne .Lsave_fail\n"
  "\tmoveq #0,d0\n\tbra .Lsave_done\n"
  ".Lsave_fail:\n\tmoveq #1,d0\n"
  ".Lsave_done:\n\taddq.l #4,sp\n"
  "\tmovem.l (sp)+,d2-d7/a2-a6\n\trts\n");

#define NOINLINE __attribute__ ((noinline, noclone))
static word NOINLINE read_d2 (word x __asm ("d2")) { return x + 1; }
static word NOINLINE read_a2 (const word *p __asm ("a2"))
{ return p[1] + 1; }
static word NOINLINE stack_arg (word x __asm ("d2"), word y)
{ return x + y; }
static word NOINLINE local_stack (word x __asm ("d2"), word y)
{ volatile word v[3]; v[0] = x; v[2] = y; return v[0] + v[2]; }
static word NOINLINE other (word x) { return x ^ 0x11223344; }
static word NOINLINE read_a6_call (const volatile word *p __asm ("a6"), word x)
{ return other (x) + p[1]; }
static word NOINLINE partial (word x __asm ("d2"))
{
  register word v __asm ("d2") = x;
  __asm__ volatile ("addq.w #1,%0" : "+d" (v));
  return v;
}
static word NOINLINE overlap (word x)
{
  register unsigned long long pair __asm ("d3") = x;
  __asm__ volatile ("" : "+r" (pair));
  return (word) pair;
}
static word NOINLINE clobber (word x)
{ __asm__ volatile ("moveq #42,d4" ::: "d4"); return x; }
static word NOINLINE increment (const word *p __asm ("a2"))
{
  word x;
  __asm__ volatile ("move.l (%0)+,%1" : "+a" (p), "=d" (x) : : "memory");
  return x;
}

int main (void)
{
  volatile word values[3];
  word x = 0, n;
  for (n = 0; n != 128; ++n)
    {
      word y = x ^ 0xa55aa55a;
      word p = (word) values;
      values[0] = x; values[1] = y; values[2] = 0x55aa55aa;
#define CHECK(F, RESULT) \
      if (check_saved ((void *) F, x, y, p, p, y, RESULT)) abort ()
      CHECK (read_d2, x + 1);
      CHECK (read_a2, y + 1);
      CHECK (stack_arg, x + y);
      CHECK (local_stack, x + y);
      CHECK (read_a6_call, (y ^ 0x11223344) + y);
      CHECK (partial, (x & 0xffff0000) | ((x + 1) & 0xffff));
      CHECK (overlap, y);
      CHECK (clobber, y);
      CHECK (increment, x);
#undef CHECK
      if (values[0] != x || values[1] != y || values[2] != 0x55aa55aa)
	abort ();
      x = x * 1664525 + 1013904223;
    }
  return 0;
}
