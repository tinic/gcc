/* { dg-do compile { target *-*-amigaos* } } */
/* { dg-options "-Os -m68020 -mno-lra -fext-dce" } */

/* Legacy reload creates address temporaries while assigning spills.
   Their saves must be included before argument/local offsets are final.
   This used to fail verify_initial_elim_offsets after reload_as_needed.  */
#include "amigaos/lra-default-ext-dce-run.c"
