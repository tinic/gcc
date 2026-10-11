/* { dg-do compile { target *-*-amigaos* } } */
/* { dg-options "-Os -m68000 -mregparm=3 -fomit-frame-pointer -fno-optimize-sibling-calls -fdump-rtl-pro_and_epilogue" } */

extern void fill (unsigned char *);
extern void consume (unsigned char *);

void conditional (unsigned long x)
{
  unsigned char b[64];
  if (!x) return;
  fill (b);
  b[x & 63] = 1;
  consume (b);
}

/* { dg-final { scan-rtl-dump "Performing shrink-wrapping" "pro_and_epilogue" } } */

