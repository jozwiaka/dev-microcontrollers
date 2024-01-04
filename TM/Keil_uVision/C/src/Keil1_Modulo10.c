#include <reg52.h>
#include <intrins.h>
#include "lcd.h"
#include "mkd51sim.h"
 unsigned char xdata PTWE _at_ 0x8008;
 unsigned char xdata PTWY _at_ 0x8009;
 unsigned char xdata PTSEG _at_ 0x8018;
 unsigned char licznik=0;
 unsigned int licz_petli=0;
 unsigned char TABLICA[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};//0-9 //gfedcba
 void main(void) {

    while(1){
        if(PTWE&2){ //true niewcisniety=true
            licznik++;
        }
        else {
            licznik--;
        }
        PTWY=licznik;
				
        for(licz_petli=0;licz_petli<10000;licz_petli++){
            PTSEG=TABLICA[licznik%10];
        }
				
    }
}
 