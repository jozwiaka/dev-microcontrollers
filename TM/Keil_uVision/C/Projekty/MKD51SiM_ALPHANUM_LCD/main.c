#include <reg52.h>
#include <intrins.h>
#include "lcd.h"
#include "mkd51sim.h"
unsigned char xdata POT0 _at_ 0x8005;  
unsigned char xdata tblcw[]="| OTO PIERWSZY |0    PROGRAM   0";
void main(void) {
unsigned char delay;
prglcd();
while(1){
	POT0=0;	
	for(delay=0;delay<200;delay++) _nop_();
	tblcw[16]='0'+POT0/100;
	disptext(tblcw);
 }
}	
 