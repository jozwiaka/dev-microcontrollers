#include "main_base.h"

int x, y;
unsigned char swieci = 1;
void ChangeLEDState(unsigned char swieci);

int main(void)
{
	RCC->AHB1ENR |= RCC_AHB1Periph_GPIOG;
	GPIOG->MODER |= GPIO_Mode_OUT << (14 * 2); // 1 - output //13
	RCC->AHB1ENR |= RCC_AHB1Periph_GPIOA;

	while (1)
	{
		if ((GPIOA->IDR & 1))
		{
			swieci = 1;
		}
		else
		{
			swieci = 0;
		}
		ChangeLEDState(swieci);
	}
}
