#include "main_base.h"

int x = 0, y = 40;//, dx = 1, dy = 1;
unsigned char bialy = 1;
unsigned short int white = 0x1f<<11 | 0x3f <<5 | 0x1F;
unsigned short int red = 0x1f<<11 | 0x00 <<5 | 0x00;
int bokKwadratu=30;
int n=0;

void move_square_asm();
unsigned short int RGB16Pack(unsigned char, unsigned char, unsigned char);

int main(void) {
    HAL_Init();
    SystemClock_Config();

    RCC->AHB1ENR |= RCC_AHB1Periph_GPIOG;
    GPIOG->MODER |= GPIO_Mode_OUT << (14*2);// 1 - output
    RCC->AHB1ENR |= RCC_AHB1Periph_GPIOA;

    while (1)
    {
        Clear_And_Reload_Screen();
        for (int yy = 0; yy < 240; yy++)
        {
        	if(yy%30==0) n++;
            unsigned short int *pixel = (unsigned short int*) (LCD_BUF + x + (y + yy) * 240);
            for (int xx = 0; xx < 240; xx++)
            {
            	if(xx%30==0) n++;


            	if(n%2==0)
            	{
            		*pixel++ = white;
            	}
            	else
            	{
            		*pixel++ = red;
            	}
            }
        }
    }
}
