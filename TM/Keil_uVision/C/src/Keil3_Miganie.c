#include <reg52.h>
#include <intrins.h>
#include "lcd.h"
#include "mkd51sim.h"

#define CLEAR_GRAPH_MEMORY 2
#define COPY_SCREEN 1

 unsigned char xdata PTWE _at_ 0x8008;   
 unsigned char xdata PTWY _at_ 0x8008;  
 unsigned char xdata PTSEG _at_ 0x8018;   

sbit P1_6 = P1^6;
int timer=0;
char light=1;

void main(void) {
	// Ustawienie licznika 0 do pracy w trybie 1 i zliczania wewnetrznych impulsów zegarowych o 
	//okresie równym 1us
	TMOD=1; //=0x01 
	TH0=(-2500)>>8; 	
	TL0=(-2500)&0xff; //=0x3c
	// Uruchomienie licznika poprzez ustawienie bitu TR0
	TCON=0x10; //TR0=1;
	
	while(1)
	{
		if(TF0)
		{
			//TR0=0; //stop timer0
			TF0=0;
			TH0=(-2500)>>8; 	
			TL0=(-2500)&0xff;
			timer++;
			//TR0=1; //start timer0
		}
		
		if(timer>=200)
		{
			timer=0;
			if(light)
			{
				P1_6=1;
				light=0;
			}
			else
			{
				P1_6=0;
				light=1;	
			}
		}
	}
}
 