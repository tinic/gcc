/* { dg-do compile } */
/* { dg-options "-m68000 -Os -mregparm=3 -fomit-frame-pointer" } */
unsigned long shift_left (void) { return 256; }
unsigned long shift_right (void) { return 0xffffff; }
unsigned long add_zero (unsigned long x) { x += 300; return x ? x : 17; }
char *pointer_offset (char *p) { return p + 300; }
unsigned long less_than (unsigned long x, unsigned long y) { return x < y; }
long less_mask (unsigned long x, unsigned long y) { return -(x < y); }
unsigned long equal_to (unsigned long x, unsigned long y) { return x == y; }
unsigned long nonzero (unsigned long x) { return x != 0; }
void copy_four (unsigned long *d, const unsigned long *s)
{ d[0]=s[0]; d[1]=s[1]; d[2]=s[2]; d[3]=s[3]; }
void copy_two (unsigned long *d, const unsigned long *s)
{ d[0]=s[0]; d[1]=s[1]; }
unsigned long word_sum (const unsigned long *p, unsigned short n)
{ unsigned long s=0; while (n--) s+=*p++; return s; }
/* { dg-final { scan-assembler {lsl\.l #2,d0} } } */
/* { dg-final { scan-assembler {lsr\.l #8,d0} } } */
/* { dg-final { scan-assembler-not {cmp\.l #-300} } } */
/* { dg-final { scan-assembler {lea \(300,a0\),a0} } } */
/* { dg-final { scan-assembler-times {move\.l \(a1\)\+,\(a0\)\+} 6 } } */
/* { dg-final { scan-assembler {subx\.l d0,d0} } } */
/* { dg-final { scan-assembler {dbra} } } */
/* No unused loop scratch should force a d2 save in these functions.  */
/* { dg-final { scan-assembler-not {move\.l d2,-\(sp\)} } } */
