#include "main_base.h"

int x, y;
void FillLCD(unsigned short int kolor);
unsigned short int red = 0x1f << 11 | 0x00 << 5 | 0x00;
int main(void)
{
	HAL_Init();

	RCC->AHB1ENR |= RCC_AHB1Periph_GPIOG;
	GPIOG->MODER |= GPIO_Mode_OUT << (14 * 2); // 1 - output //13
	RCC->AHB1ENR |= RCC_AHB1Periph_GPIOA;

	while (1)
	{
		FillLCD(red);
		Clear_And_Reload_Screen();
	}
}
