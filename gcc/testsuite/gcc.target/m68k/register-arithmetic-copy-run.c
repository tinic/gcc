/* { dg-do run } */
/* { dg-options "-O2 -fomit-frame-pointer" } */
/* Exercise the same value/liveness/overflow cases with the board's CPU
   and ordinary C ABI, including on non-Amiga targets.  */
#include "amigaos-register-arithmetic-copy-run.c"
