/* { dg-do compile { target *-*-amigaos* } } */
/* { dg-options "-O2 -m68020 -mregparm=3 -fomit-frame-pointer -fno-optimize-sibling-calls" } */

/* Do not introduce long PC-relative call relocations on newer CPUs.  */
#include "amigaos-short-calls.c"

/* { dg-final { scan-assembler-not {jbsr} } } */
/* { dg-final { scan-assembler-times {jsr _} 3 } } */
