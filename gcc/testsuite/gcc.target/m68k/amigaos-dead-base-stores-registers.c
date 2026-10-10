/* Additional native ABI validation: address-register arguments, memory
   aliases, pointer liveness and even-but-not-longword-aligned storage. */
/* { dg-do compile } */
/* { dg-skip-if "Amiga register parameters" { ! { m68k-*-amigaos* } } } */
/* { dg-options "-m68000 -Os -mregparm=3 -fomit-frame-pointer" } */
typedef unsigned long word;
struct block { word a, b, c, d; };
#define STORE_FN(N) \
void init_a##N (struct block *p __asm("a" #N), word x __asm("d0"), \
               word y __asm("d1")) \
{ p->a = x; p->b = y; p->c = 0; p->d = 0; }
STORE_FN(0)
STORE_FN(1)
STORE_FN(2)
STORE_FN(3)
STORE_FN(4)
STORE_FN(6)

word retain_base (struct block *p, word x, word y)
{ p->a = x; p->b = y; p->c = 0; p->d = 0; return (word)p; }
void store_base (struct block *p, word x)
{ p->a=x; p->b=(word)p; p->c=0; p->d=(word)p; }
void shifted (struct block *p, word x, word y)
{ ++p; p->a=x; p->b=y; p->c=0; p->d=0; }
void neighboring (struct block *p, word x, word y)
{ p->a=x; p->b=y; p->c=p[1].a; p->d=0; }
void source_alias (struct block *p, const struct block *q)
{ p->a=q->a; p->b=q->b; p->c=q->c; p->d=q->d; }
void volatile_widths (volatile struct block *p, word x, word y)
{ p->a=x; p->b=y; p->c=0; p->d=0; }
