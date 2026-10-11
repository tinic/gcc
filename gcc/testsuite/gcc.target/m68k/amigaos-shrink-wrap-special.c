/* { dg-do compile { target *-*-amigaos* } } */
/* { dg-options "-Os -m68000 -mregparm=3 -fomit-frame-pointer -fno-optimize-sibling-calls -fdump-rtl-pro_and_epilogue" } */

extern void fill (unsigned char *);
extern void consume (unsigned char *);

#define BODY \
  unsigned char b[64]; \
  if (!x) return; \
  fill (b); b[x & 63] = 1; consume (b);

__attribute__((saveallregs)) void save_all (unsigned long x) { BODY }

/* { dg-final { scan-rtl-dump-not "Attempting shrink-wrapping" "pro_and_epilogue" } } */
