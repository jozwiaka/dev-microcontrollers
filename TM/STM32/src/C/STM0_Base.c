#include "main_base.h"

int x = 120, y = 160, dx = 1, dy = 1;

void move_square_asm();
unsigned short int RGB16Pack(unsigned char, unsigned char, unsigned char);

void move_square_C()
{

	x += dx;
	y += dy;
	if (x <= 1 || x >= 240 - 34)
		dx = -dx;
	if (y <= 1 || y >= 320 - 34)
		dy = -dy;
}

int main(void)
{
	HAL_Init();
	SystemClock_Config();

#ifndef STMSIM
	BSP_SDRAM_Init();
	LCD_Config();
#endif

	RCC->AHB1ENR |= RCC_AHB1Periph_GPIOG;
	GPIOG->MODER |= GPIO_Mode_OUT << (14 * 2); // 1 - output
	RCC->AHB1ENR |= RCC_AHB1Periph_GPIOA;

	while (1)
	{
		if (!(GPIOA->IDR & 1))
			GPIOG->BSRR = GPIO_Pin_14; //0x4000
		else
			GPIOG->BSRR = GPIO_Pin_14 << 16; //0x40000000

		Clear_And_Reload_Screen();

#ifdef ASM_EXMPL
		move_square_asm();
#else
		move_square_C();
#endif

		for (int yy = 0; yy < 32; yy++)
		{
			unsigned short int *pixel = (unsigned short int *)(LCD_BUF + x + (y + yy) * 240);
			for (int xx = 0; xx < 32; xx++)
			{
#ifdef ASM_EXMPL
				*pixel++ = RGB16Pack(y >> 4, (xx + (((240 - x)) * (32 - yy) / 240) + x * yy / 240), 31 - (x >> 4));
#else
				*pixel++ = ((31 - ((x >> 4) & 0x1f)) << 11) | (((xx + (((240 - x)) * (32 - yy) / 240) + x * yy / 240) & 0x3f) << 5) | (((y >> 4) & 0x1f) << 0);
#endif
			}
		}
	}
}
