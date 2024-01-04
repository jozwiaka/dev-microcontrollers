#include <reg52.h>
#include <intrins.h>
#include "lcd.h"
#include "mkd51sim.h"

unsigned char xdata POT0 _at_ 0x8005;
unsigned char xdata POT1 _at_ 0x8006;
unsigned char xdata POT2 _at_ 0x8007;
unsigned char xdata PTWE _at_ 0x8008;
unsigned char xdata PTWY _at_ 0x8009;
unsigned char xdata tblcw[]="Napiecie = 0   V";
void main(void) {
unsigned char delay,POT;
float j=0,d1=0,d2=0;
prglcd();
while(1){
	if(!(PTWE&0x01))
	{
		POT0=0;
		for(delay=0;delay<200;delay++) _nop_();
		POT=POT0;
	}
	if(!(PTWE&0x02))
	{
		POT1=0;
		for(delay=0;delay<200;delay++) _nop_();
		POT=POT1;
	}
	if(!(PTWE&0x04))
	{
		POT2=0;
		for(delay=0;delay<200;delay++) _nop_();
		POT=POT2;
	}
	j=(float)POT/51; //0-5
	tblcw[11]='0'+j;
	tblcw[12]='.';
	d1=(j-(int)j)*10; //0.0-0.9       1.54345-1=0.5435 -> 5.435 //explicit cast
	d2=(d1-(int)d1)*10; //0.00-0.09		5.435-5=0.435 -> 4.35
	tblcw[13]='0'+ (unsigned char)d1; //5.435->5 //implicit cast
	tblcw[14]='0'+ d2;								//4.35 ->4
	disptext(tblcw);
}
}
 