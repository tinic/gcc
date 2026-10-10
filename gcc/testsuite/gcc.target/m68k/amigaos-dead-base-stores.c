/* Adjacent stores through a dead address register can use postincrement.  */
/* { dg-do compile } */
/* { dg-skip-if "Amiga register parameters" { ! { m68k-*-amigaos* } } } */
/* { dg-options "-m68000 -Os -mregparm=3 -fomit-frame-pointer -fno-optimize-sibling-calls" } */

struct cursor
{
  void *vec;
  long count;
  unsigned long index;
  unsigned long offset;
};

void
init_cursor (struct cursor *p, void *vec, long count)
{
  p->vec = vec;
  p->count = count;
  p->index = 0;
  p->offset = 0;
}

/* { dg-final { scan-assembler-times "\\(a0\\)\\+" 3 } } */
