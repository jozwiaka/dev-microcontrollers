#include "main_base.h"

int x = 0, y = 0;

unsigned short int RGB16Pack(unsigned char, unsigned char, unsigned char);
void DrawHLine(void *adres, unsigned int pozX, unsigned int pozY, unsigned short int kolor);

unsigned int pozX = 50;
unsigned int pozY = 50;
unsigned short int red = 0x1f << 11 | 0x00 << 5 | 0x00;
int main(void)
{
	HAL_Init();

	RCC->AHB1ENR |= RCC_AHB1Periph_GPIOG;
	GPIOG->MODER |= GPIO_Mode_OUT << (14 * 2); // 1 - output
	RCC->AHB1ENR |= RCC_AHB1Periph_GPIOA;

	while (1)
	{
		DrawHLine((void *)LCD_BUF, pozX, pozY, red);
		Clear_And_Reload_Screen();
	}
}