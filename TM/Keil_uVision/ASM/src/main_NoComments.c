#include <reg52.h>
#include <intrins.h>
#include "lcd.h"
#include "mkd51sim.h"
#define MixedAsm
#ifdef MixedAsm
void INWERSJAP1_6();
void WAIT_10US(int wait);
unsigned char LONG_PARAM_LIST(long l1, long l2);
#endif
unsigned char xdata PTWE _at_ 0x8008;
unsigned char xdata PTWY _at_ 0x8009;
unsigned char xdata PTSEG _at_ 0x8018;
unsigned char xdata POT0 _at_ 0x8005;
unsigned char xdata LCDWRITE _at_ 0x7fff;
#define CLEAR_GRAPH_MEMORY 2
#define COPY_SCREEN 1
sbit P1_6 = P1 ^ 6;
sbit P1_7 = P1 ^ 7;
unsigned char xdata tblcw[] = "| OTO PIERWSZY |0    PROGRAM   0";
unsigned char presc = 0;
unsigned char Modul7Seg = 0;
unsigned char soundtim = 0;
bit flag = 0;
data int ZEZWOLENIE_L8 = 1;
unsigned int x = 30, y = 80;
unsigned int xx, yy;
char dx = 1, dy = -1;
unsigned char xdata *pixel;
void main(void)
{
#ifdef MixedAsm
    PTWY = LONG_PARAM_LIST(0, 0x81);
    WAIT_10US(-50000);
#endif
    P1 = 0x3c;
    TMOD = 1;
    TH0 = (-2500) >> 8;
    TL0 = (-2500) & 0xff;
    IE = 0x82;
    TCON = 0x10;
    prglcd();
    POT0 = 0;
    while (1)
    {
#ifdef MixedAsm
        WAIT_10US(-100);
#else
        unsigned int delay;
        for (delay = 0; delay < 200; delay++)
            _nop_();
#endif
        disptext(tblcw);
        if (flag)
        {
            flag = 0;
            x += dx;
            y += dy;
            if (y == 0 || y == 107)
                dy = -dy;
            if (x == 0 || x == 43)
                dx = -dx;
            LCDWRITE = CLEAR_GRAPH_MEMORY;
            for (yy = 0; yy < 20; yy++)
            {
                pixel = (unsigned char xdata *)(0xa000 + x * 3 + (y + yy) * 64 * 3);
                for (xx = 0; xx < 20; xx++)
                {
                    *(pixel++) = x * 4;
                    *(pixel++) = xx * 6 + yy * 6;
                    *(pixel++) = y * 2;
                }
            }
            LCDWRITE = COPY_SCREEN;
        }
    }
}
void IRT_TIMER0() interrupt 1
{
    TH0 = (-2500) >> 8;
    TL0 = (-2500) & 0xff;
    flag = 1;
    if (++presc == 200)
    {
        presc = 0;
#ifdef MixedAsm
        INWERSJAP1_6();
#else
        P1_6 = !P1_6;
#endif
        tblcw[16]++;
        tblcw[16] &= 48 + 7;
        tblcw[31] = tblcw[16];
        P1_7 = 1;
        soundtim = 30;
    }
    if (soundtim > 0)
        soundtim--;
    else
        P1_7 = 0;
#ifndef Persistent_7sLED
    PTSEG = 0;
#endif
    Modul7Seg = ++Modul7Seg & 3;
    PTSEG = 0;
    P1 = (P1 & 0xfc) | Modul7Seg | 0x3c;
    PTSEG = Modul7Seg | (~P1 & 0x7c) | ((~PTWE << (7 - Modul7Seg)) & 0x80);
    PTWY = POT0;
    POT0 = 0;
}
