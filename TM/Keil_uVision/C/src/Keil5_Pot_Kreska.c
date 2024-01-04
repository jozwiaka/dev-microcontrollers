#include <reg52.h>
#include <intrins.h>
#include "lcd.h"
#include "mkd51sim.h"

unsigned char xdata POT2 _at_ 0x8007;
unsigned char xdata PTWE _at_ 0x8008;
unsigned char xdata PTWY _at_ 0x8009;
unsigned char xdata tblcw[]="                           ";
void main(void) {
unsigned char delay, POT;
unsigned int miejsce;
	int i =0;
prglcd();
while(1){
    POT2 = 0;
		for(delay=0;delay<200;delay++)_nop_();
		POT = POT2;
		miejsce = (int)POT/17;
		for(i=0;i<16;i++)
	{
			if(i==miejsce) continue;
			tblcw[i]=' ';
	}
		tblcw[miejsce]='|';
	
	disptext(tblcw);
	}
}

		
 