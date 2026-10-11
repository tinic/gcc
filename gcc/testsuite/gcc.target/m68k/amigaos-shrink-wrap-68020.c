/* { dg-do compile { target *-*-amigaos* } } */
/* { dg-options "-Os -m68020 -mregparm=3 -fomit-frame-pointer -fno-optimize-sibling-calls -fdump-rtl-pro_and_epilogue" } */

#include "amigaos-shrink-wrap.c"

/* { dg-final { scan-rtl-dump-not "Attempting shrink-wrapping" "pro_and_epilogue" } } */
