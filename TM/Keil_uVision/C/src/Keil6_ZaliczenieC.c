#include <reg52.h>
#include <intrins.h>
#include "lcd.h"
#include "mkd51sim.h"

unsigned char xdata POT1 _at_ 0x8006;
unsigned char xdata POT2 _at_ 0x8007;
unsigned char xdata PTWE _at_ 0x8008;
unsigned char xdata PTWY _at_ 0x8009;
unsigned char xdata tblcw[] = "                               ";
void main(void)
{
    unsigned char delay, pt1, pt2, roznica;
    float j = 0, d1 = 0, d2 = 0;
    prglcd();
    while (1)
    {
        POT1 = 0;
        for (delay = 0; delay < 200; delay++)
            _nop_();
        pt1 = POT1;

        POT2 = 0;
        for (delay = 0; delay < 200; delay++)
            _nop_();
        pt2 = POT2;
        if (pt1 >= pt2)
        {
            roznica = pt1 - pt2;
            tblcw[10] = ' ';
        }
        else
        {
            roznica = pt2 - pt1;
            tblcw[10] = '-';
        }
        j = (float)roznica / 51;
        tblcw[11] = '0' + j;
        tblcw[12] = '.';
        d1 = (j - (int)j) * 10;
        d2 = (d1 - (int)d1) * 10;
        tblcw[13] = '0' + (unsigned char)d1;
        tblcw[14] = '0' + d2;
        disptext(tblcw);
    }
}