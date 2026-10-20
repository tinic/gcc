/* { dg-do compile } */
/* { dg-options "-m68000 -Os -mregparm=3" } */
/* Preserve the ordered byte accesses and fold the final zero extension
   into the existing byte-OR instruction, not a wider volatile access.  */
unsigned long pack_bytes (const volatile unsigned char *p)
{
  unsigned long a=p[0], b=p[2], c=p[4], d=p[6];
  return (a<<24) | (b<<16) | (c<<8) | d;
}
/* { dg-final { scan-assembler {or\.b 6\(a0\),d[0-7]} } } */
/* { dg-final { scan-assembler-not {move\.[wl] [0246]\(a0\)} } } */
