#include <reg52.h>
#include <intrins.h>
#include "lcd.h"
#include "mkd51sim.h"

unsigned char xdata PTWY _at_ 0x8009;
unsigned char xdata PTWE _at_ 0x8008;

void ShowButtonOnL8(unsigned char nr_przycisku);
void main(void)
{

	while (1)
	{
		ShowButtonOnL8(3);
	}
}