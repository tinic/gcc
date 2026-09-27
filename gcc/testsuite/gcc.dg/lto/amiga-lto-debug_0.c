/* { dg-lto-do link } */
/* { dg-skip-if "" { ! { m68k-*-amigaos* } } } */
/* { dg-lto-options { { -flto -g -mcrt=nix20 } } } */

int
main (void)
{
  return 0;
}
