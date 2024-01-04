#include "main_base.h"

int x = 120, y = 160, dx = 1, dy = 1;
int yChart1 = 120;
int yChart2 = 90;
float scale1 = 240 * 0.01;
float scale2 = 240 / 60;
unsigned int width = 10;
unsigned int mTimeDefined = 0;
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
struct s_GlobalTime
{
    unsigned int ms;
    unsigned int s;
    unsigned int min;
};
struct s_GlobalTime GlobalTime={0,0,0};
void IRT_SYSTICK()
{
    //aktualizacja GlobalTime
    GlobalTime.ms++;
    if (GlobalTime.ms == 100)
    {
        GlobalTime.s++;
        GlobalTime.ms = 0;
    }
    if(GlobalTime.s==60)
    {
        GlobalTime.s=0;
    }
}
unsigned short int black = 0x00 << 11 | 0x00 << 5 | 0x00;
unsigned short int grey = 0x00 << 11 | 0x00 << 5 | 0x1f;
unsigned short int yellow = 0x1f << 11 | 0x3f << 5 | 0x00;
int main(void)
{
    //w main_base.c dezaktywowac while'a 31 min
    *((unsigned long *)(SCB->VTOR + 0x3c)) = (unsigned long)IRT_SYSTICK;
    //kofniguracja SYSTICK-a
    SysTick->CTRL = 7;
    //SysTick->LOAD = 16000000 - 1; //zegar 16megaHercw
    SysTick->LOAD = 160000 - 1; //zegar 16megaHercw
    while (1)
    {
        // <---- wylacz INT
        __disable_irq();
        // ....krytyczny kod
        struct s_GlobalTime mTime = GlobalTime;
        mTimeDefined = 1;
        __enable_irq();
        // <----- wlacz INT

        //Wizualizacje graficznej reprezentacji up�ywu czasu
        //na podstawie mTime
        //----------------------------------------
        Clear_And_Reload_Screen();

        move_square_C();
        if (mTimeDefined)
        {
            //Wykresy
            for (int yy = 0; yy < 320; yy++)
            {
                unsigned short int *pixel = (unsigned short int *)(LCD_BUF + yy * 240);
                for (int xx = 0; xx < 240; xx++)
                {
                    //ms
                    if (yy >= yChart1 - width && yy <= yChart1 + width)
                    {
                        if (xx >= 240 - mTime.ms * scale1)
                        {
                            *pixel++ = yellow;
                        }
                        else
                        {
                            *pixel++ = black;
                        }
                    }
                    else if (yy >= yChart2 - width && yy <= yChart2 + width)
                    {
                        if (xx >= 240 - mTime.s * scale2)
                        {
                            *pixel++ = grey;
                        }
                        else
                        {
                            *pixel++ = black;
                        }
                    }
                    else
                    {
                        *pixel++ = black;
                    }
                }
                //kwadrat
                for (int yy = 0; yy < 32; yy++)
                {
                    unsigned short int *pixel = (unsigned short int *)(LCD_BUF + x + (y + yy) * 240);
                    for (int xx = 0; xx < 32; xx++)
                    {
                        *pixel++ = ((31 - ((x >> 4) & 0x1f)) << 11) | (((xx + (((240 - x)) * (32 - yy) / 240) + x * yy / 240) & 0x3f) << 5) | (((y >> 4) & 0x1f) << 0);
                    }
                }
            }
        }
    }
}
