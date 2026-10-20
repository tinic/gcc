/* { dg-do compile } */
/* { dg-options "-m68000 -Os -mregparm=3" } */
extern int f1(void*),f2(void*),f3(void*),f4(void*),f5(void*),f6(void*);
int dispatch(unsigned short n,void*p) {
 switch(n) { case 1:return f1(p);case 2:return f2(p);case 3:return f3(p);
 case 4:return f4(p);case 5:return f5(p);case 6:return f6(p);default:return -1; }
}
/* { dg-final { scan-assembler-not {and\.l #65535} } } */
/* { dg-final { scan-assembler {pc,d[0-7]\.w} } } */
