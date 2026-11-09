/* { dg-do compile } */
/* { dg-options "-Os -m68000" } */

struct state {
  int a, b, c, d, e;
  int buf[8];
};

static struct state g_state;
void touch (int *);

int
update_state (int x)
{
  g_state.a = x;
  g_state.b = x + 1;
  touch (g_state.buf);
  return g_state.c + g_state.d + g_state.e;
}

/* { dg-final { scan-assembler {lea _g_state,a} } } */
/* { dg-final { scan-assembler-not {_g_state\+} } } */
