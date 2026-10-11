/* { dg-do compile { target *-*-amigaos* } } */
/* { dg-options "-Os -m68000 -mregparm=3 -fbaserel -fomit-frame-pointer -fno-optimize-sibling-calls -fdump-rtl-pro_and_epilogue" } */

extern void fill (unsigned char *);
extern void consume (unsigned char *);

#define BODY \
  unsigned char b[64]; \
  if (!x) return; \
  fill (b); b[x & 63] = 1; consume (b);

void ordinary (unsigned long x) { BODY }
__attribute__((saveds)) void saved (unsigned long x) { BODY }

/* { dg-final { scan-rtl-dump-not "Attempting shrink-wrapping" "pro_and_epilogue" } } */
