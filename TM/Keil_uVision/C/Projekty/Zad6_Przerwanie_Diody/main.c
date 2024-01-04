#include <reg52.h>
#include <intrins.h>
#include "lcd.h"
#include "mkd51sim.h"

unsigned char xdata PTSEG _at_ 0x8018;
unsigned char wyswietlacz[4];
unsigned char presc = 0, seg = 0;
unsigned char Tab7s[] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f, 0x77, 0x7c, 0x39, 0x5f, 0x79, 0x71};

unsigned int licznik = 9960; //16 bitow

void main(void)
{

	P1 = 0x3c;
	TMOD = 1;
	IE = 0x82;			  //odblokoweanie przerwan
	TH0 = (-2500) >> 8;	  // czestotliwosc przerwan- 2,5ms
	TL0 = (-2500) & 0xff; //
	TCON = 0x10;

	while (1)
	{
		if (licznik == 10000)
		{
			licznik = 0;
		}
		wyswietlacz[0] = licznik % 10;
		wyswietlacz[1] = licznik / 10 % 10;
		wyswietlacz[2] = licznik / 100 % 10;
		wyswietlacz[3] = licznik / 1000 % 10;	
	}
}

void a() interrupt 1
{
	TH0 = (-2500) >> 8;
	TL0 = (-2500) & 0xff;

	if (++presc == 100) //szybkosc modulo 10000
	{
		presc = 0;
		licznik++;
	}
	seg = ++seg & 3;
	P1 = (P1 & 0xfc) | seg | 0x3c;
	PTSEG = Tab7s[wyswietlacz[seg]];
}
