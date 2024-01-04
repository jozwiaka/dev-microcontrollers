#include <reg52.h>
#include <intrins.h>
#include "mkd51sim.h"
#include "lcd.h"

unsigned char xdata PTWE _at_ 0x8008;
unsigned char xdata PTWY _at_ 0x8009;
unsigned char xdata PTSEG _at_ 0x8018;

void licz();
void main(void)
{
	while (1)
	{

		licz();
	}
}
