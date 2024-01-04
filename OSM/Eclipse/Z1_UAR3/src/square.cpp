#include "setup.h"
#include "square.h"
#include "stdlib.h"


extern unsigned long *ekran;

square::square()
{
      s=(rand()&0x1f)+1;
      x=rand()&0x03ff;  if(x>239*4-s) x-=239*4-s;
      y=rand()&0x1ff;     if(y>127*4-s) y-=127*4-s;
      dx=((rand()&0x1) ? 1 : -1) * (((rand()&0x7)+2)>>1);
      dy=((rand()&0x1) ? 1 : -1) * (((rand()&0x7)+2)>>1);

}
void square::move (int dax,int day)
{
	      y+=dy+6-day;
	      x+=dx+6-dax;
	      if(x<0) x+=239*4-s;
	      if(y<0) y+=127*4-s;
	      if(x>239*4-s) x-=239*4-s;
	      if(y>127*4-s) y-=127*4-s;
}

void square::draw ()
{
    long sdec=(((x-120*4L)*(x-120*4L))>>13)+(((y-58*4L)*(y-58*4L))>>9);
          long size=s-sdec;
          if(size<0) size=0;
          size=s-size;
  for (int b=y>>2;b<(y+size)>>2;b++)
  for (int a=x>>2;a<(x+size)>>2;a++)
	  SetPixel(ekran,a,b);

}
