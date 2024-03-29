#include "main_base.h"

int x, y;
void DrawSquare(unsigned short int x, unsigned short y, unsigned short int kolor);
unsigned short int lX = 240 / 2;
unsigned short int lY = 320 / 2;
unsigned short int red = 0x1f << 11 | 0x00 << 5 | 0x00;
int main(void)
{
	HAL_Init();

	RCC->AHB1ENR |= RCC_AHB1Periph_GPIOG;
	GPIOG->MODER |= GPIO_Mode_OUT << (14 * 2); // 1 - output //13
	RCC->AHB1ENR |= RCC_AHB1Periph_GPIOA;

	while (1)
	{
		DrawSquare(lX, lY, red);
		Clear_And_Reload_Screen();
	}
}
