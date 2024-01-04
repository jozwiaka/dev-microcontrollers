#include <reg52.h>
#include <intrins.h>
#include "lcd.h"
#include "mkd51sim.h"
 unsigned char xdata PTWE _at_ 0x8008;   
 unsigned char xdata PTWY _at_ 0x8009;  
 unsigned char xdata PTSEG _at_ 0x8018;   
sbit P1_6 = P1^6;  
 unsigned char licznik=0;
 unsigned int licz_petli=0;
 void main(void) {
	
	while(1){
		for(licz_petli=0;licz_petli<10000;licz_petli++) _nop_();
		if (PTWE&1) PTSEG=licznik++;
	}
}
 


 