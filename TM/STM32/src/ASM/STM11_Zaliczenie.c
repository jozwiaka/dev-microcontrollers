#include "main_base.h"

int x, y;
void DrawRectCorners(unsigned short int x, unsigned short y);
unsigned short int pozX = 100;
unsigned short int pozY = 100;
unsigned short int red = 0x1f << 11 | 0x00 << 5 | 0x00;
int main(void)
{
	HAL_Init();

	RCC->AHB1ENR |= RCC_AHB1Periph_GPIOG;
	GPIOG->MODER |= GPIO_Mode_OUT << (14 * 2); // 1 - output //13
	RCC->AHB1ENR |= RCC_AHB1Periph_GPIOA;

	while (1)
	{
		DrawRectCorners(pozX, pozY);
		Clear_And_Reload_Screen();
	}
}
