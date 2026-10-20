/* { dg-do run } */
/* { dg-options "-m68000 -Os" } */
#include <stdlib.h>
static volatile unsigned long observed;
__attribute__((noipa)) unsigned long count(unsigned long n) {
 unsigned long sum=0; while(n--) { ++sum; observed=sum; } return sum;
}
int main(void) {
 static const unsigned long values[]={0,1,2,32767,32768,65535,65536,65537,131073};
 for(unsigned i=0;i<sizeof(values)/sizeof(values[0]);++i)
   if(count(values[i])!=values[i]) abort();
 return 0;
}
