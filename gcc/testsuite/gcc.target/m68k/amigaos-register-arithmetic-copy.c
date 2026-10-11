/* { dg-do compile { target *-*-amigaos* } } */
/* { dg-options "-Os -m68000 -mregparm=3 -fomit-frame-pointer" } */

struct pair { unsigned long a, b; };
struct pair add_pair (unsigned long x, unsigned long y)
{
  struct pair p = { x + 1, y + 2 };
  return p;
}

/* { dg-final { scan-assembler "addq\\.l #1,d0" } } */
/* { dg-final { scan-assembler "addq\\.l #2,d1" } } */
/* { dg-final { scan-assembler-not "move\\.l d2,d[01]" } } */

