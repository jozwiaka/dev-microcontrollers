#include "main_base.h"

int x, y;

void ReverseDiodes();

int main(void)
{
	RCC->AHB1ENR |= RCC_AHB1Periph_GPIOG;
	//GPIOG->MODER |= GPIO_Mode_OUT << (14 * 2);
	RCC->AHB1ENR |= RCC_AHB1Periph_GPIOA;

	while (1)
	{
		ReverseDiodes();
	}
}
