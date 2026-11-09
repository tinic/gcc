/* { dg-do compile } */
/* { dg-options "-Os -m68000 -fomit-frame-pointer" } */

struct node {
  struct node *next;
  int val;
};

int
walk (struct node *p)
{
  if (!p)
    return -1;
  struct node *n = p->next;
  if (!n)
    return p->val;
  return n->val;
}

/* { dg-final { scan-assembler-not {cmp\.w #0,a} } } */
