#include "main_base.h"

int x = 0, y = 0; //, dx = 1, dy = 1;
int licznik = 0;
int R = 0;
int G = 0;
int B = 0;
int skala = 200;
unsigned char zmiana = 0;

void move_square_asm();
unsigned short int RGB16Pack(unsigned char, unsigned char, unsigned char);

unsigned short int black = 0x00 << 11 | 0x00 << 5 | 0x00;
unsigned short int blue = 0x00 << 11 | 0x00 << 5 | 0x1F;
int main(void)
{
    HAL_Init();
    SystemClock_Config();

    RCC->AHB1ENR |= RCC_AHB1Periph_GPIOG;
    GPIOG->MODER |= GPIO_Mode_OUT << (14 * 2); // 1 - output
    RCC->AHB1ENR |= RCC_AHB1Periph_GPIOA;

    while (1)
    {
        Clear_And_Reload_Screen();
        if (licznik == skala)
        {
            zmiana = 1;
        }
        if (licznik == 1)
        {
            zmiana = 0;
        }
        R = licznik * 0x1F / skala; //k=0->1
        G = licznik * 0x3F / skala;
        B = 0x1F; //k=1->0

        for (int yy = 0; yy < 320; yy++)
        {
            unsigned short int *pixel = (unsigned short int *)(LCD_BUF + x + (y + yy) * 240);
            for (int xx = 0; xx < 240; xx++)
            {

                if (xx >= yy - 10 && xx <= yy + 10)
                {
                    *pixel++ = R << 11 | G << 5 | B;
                }
                else
                {
                    *pixel++ = black;
                }
            }
        }
        if ((GPIOA->IDR & 1))
        {
            if (zmiana)
                licznik--;
            else
                licznik++;
        }
    }
}
