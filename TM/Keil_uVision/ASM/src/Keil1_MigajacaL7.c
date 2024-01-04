#include <reg52.h>
#include <intrins.h>
#include "mkd51sim.h"

void MOJA_PROC_W_ASM();
void INIT_ASM();
extern char ZM;

// Deklaracje rejestrow sprzetowych MKD51
unsigned char xdata PTWE _at_ 0x8008;
unsigned char xdata PTWY _at_ 0x8009;
unsigned char xdata PTSEG _at_ 0x8018;
unsigned char xdata POT0 _at_ 0x8005;
void main(void)
{
	INIT_ASM();
	while (1)
	{
		MOJA_PROC_W_ASM();
	}
}
