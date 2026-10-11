/* { dg-do compile { target *-*-amigaos* } } */
/* { dg-options "-Os -m68000 -mregparm=3 -fomit-frame-pointer" } */

unsigned long extract (unsigned long x)
{
  return ((x >> 5) & 7) | ((x >> 17) & 31);
}

/* Each MOVEQ plus register AND is four bytes; folding into an immediate
   longword AND would take six bytes and keep the allocated D2 saves.  */
/* { dg-final { scan-assembler "moveq #7,d2" } } */
/* { dg-final { scan-assembler "moveq #31,d2" } } */
/* { dg-final { scan-assembler-not "and\\.l #7" } } */
/* { dg-final { scan-assembler-not "and\\.l #31" } } */

