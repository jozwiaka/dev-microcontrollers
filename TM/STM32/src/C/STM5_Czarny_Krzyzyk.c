#include "main_base.h"

int x = 120, y = 160, dx = 1, dy = 0;

void move_square_asm();
unsigned short int RGB16Pack(unsigned char, unsigned char, unsigned char);

void poruszanie_krzyzyka()
{

    x += dx;
    y += dy;
    if (x <= 1 || x >= 240 - 34)
        dx = -dx;
    if (y <= 1 || y >= 320 - 34)
        dy = -dy;
}
unsigned short int red = 0x1f << 11 | 0x00 << 5 | 0x00;
unsigned short int black = 0x00 << 11 | 0x00 << 5 | 0x00;
int main(void)
{
    HAL_Init();
    SystemClock_Config();

    RCC->AHB1ENR |= RCC_AHB1Periph_GPIOG;
    GPIOG->MODER |= GPIO_Mode_OUT << (14 * 2); // 1 - output
    RCC->AHB1ENR |= RCC_AHB1Periph_GPIOA;

    SysTick->CTRL = 7;
    SysTick->LOAD = 388350 - 1; //16000000*5/(240-34)
    while (1)
    {
        Clear_And_Reload_Screen();
        poruszanie_krzyzyka();

        for (int yy = 0; yy < 320; yy++)
        {
            unsigned short int *pixel = (unsigned short int *)(LCD_BUF + yy * 240);
            for (int xx = 0; xx < 240; xx++)
            {
                *pixel++ = red;
            }
        }

        for (int yy = 0; yy < 32; yy++)
        {
            unsigned short int *pixel = (unsigned short int *)(LCD_BUF + x + (y + yy) * 240);

            for (int xx = 0; xx < 32; xx++)
            {
                if ((xx == yy || xx == 31 - yy))
                    *pixel++ = black;
                else
                {
                    *pixel++ = red;
                }
            }
        }
    }
}
