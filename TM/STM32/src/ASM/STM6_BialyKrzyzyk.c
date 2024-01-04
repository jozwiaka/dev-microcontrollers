#include "main_base.h"

int x, y;
void DrawCross(unsigned short int x, unsigned short y);
unsigned short int pozX = 240 / 2;
unsigned short int pozY = 320 / 2;
int main(void)
{
	HAL_Init();

	RCC->AHB1ENR |= RCC_AHB1Periph_GPIOG;
	GPIOG->MODER |= GPIO_Mode_OUT << (14 * 2); // 1 - output //13
	RCC->AHB1ENR |= RCC_AHB1Periph_GPIOA;

	while (1)
	{
		DrawCross(pozX, pozY);
		Clear_And_Reload_Screen();
	}
}
