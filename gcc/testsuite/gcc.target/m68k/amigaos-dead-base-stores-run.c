/* Preserve the original pointer when it is returned or stored as a value,
   and preserve store widths/order for volatile objects.  */
/* { dg-do run } */
/* { dg-skip-if "Amiga register parameters" { ! { m68k-*-amigaos* } } } */
/* { dg-options "-m68000 -Os -mregparm=3 -fomit-frame-pointer -fno-optimize-sibling-calls" } */

extern void abort (void) __attribute__ ((noreturn, stkparm));
typedef unsigned long word;
struct block { word a, b, c, d; };

static void __attribute__ ((noinline, noclone))
write4 (struct block *p, word x, word y)
{
  p->a = x;
  p->b = y;
  p->c = 0;
  p->d = 0;
}

static struct block * __attribute__ ((noinline, noclone))
retain (struct block *p, word x, word y)
{
  p->a = x;
  p->b = y;
  p->c = 0;
  p->d = 0;
  return p;
}

static void __attribute__ ((noinline, noclone))
alias (struct block *p, word x)
{
  p->a = x;
  p->b = (word) p;
  p->c = 0;
  p->d = (word) p;
}

static void __attribute__ ((noinline, noclone))
volatile4 (volatile struct block *p, word x, word y)
{
  p->a = x;
  p->b = y;
  p->c = 0;
  p->d = 0;
}

int
main (void)
{
  struct { word before; struct block p; word after; } check;
  check.before = 0x55aa55aa;
  check.after = 0xaa55aa55;
  write4 (&check.p, 0x12345678, 0x87654321);
  if (check.p.a != 0x12345678 || check.p.b != 0x87654321
      || check.p.c != 0 || check.p.d != 0)
    abort ();
  if (retain (&check.p, 1, 2) != &check.p
      || check.p.a != 1 || check.p.b != 2 || check.p.c || check.p.d)
    abort ();
  alias (&check.p, 3);
  if (check.p.a != 3 || check.p.b != (word) &check.p
      || check.p.c != 0 || check.p.d != (word) &check.p)
    abort ();
  volatile4 (&check.p, 4, 5);
  if (check.p.a != 4 || check.p.b != 5 || check.p.c || check.p.d
      || check.before != 0x55aa55aa || check.after != 0xaa55aa55)
    abort ();
  return 0;
}
