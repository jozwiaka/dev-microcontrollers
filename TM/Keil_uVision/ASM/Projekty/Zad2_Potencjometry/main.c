#include <reg52.h>
#include <intrins.h>
#include "lcd.h"
#include "mkd51sim.h"

unsigned char xdata POT0 _at_ 0x8005;
unsigned char xdata POT1 _at_ 0x8006;
unsigned char xdata POT2 _at_ 0x8007;
unsigned char xdata PTWE _at_ 0x8008;
unsigned char xdata tblcw[] = "Napiecie = 0   V";
unsigned char POT;
float f=0;
unsigned char GETADC(unsigned char Nr_POT);
void main(void)
{
    prglcd();
    while (1)
    {
        if (!(PTWE & 0x01))
        {
            POT0 = 0;
            POT = GETADC(0);
        }
        if (!(PTWE & 0x02))
        {
            POT1 = 0;
            POT = GETADC(1);
        }
        if (!(PTWE & 0x04))
        {
            POT2 = 0;
            POT = GETADC(2);
        }
        f = (float)POT / 51;
        tblcw[11] = '0' + f;
        tblcw[12] = '.';
        f = (f - (int)f) * 10;
        tblcw[13] = '0' + f;
        f = (f - (int)f) * 10;
        tblcw[14] = '0' + f;
        disptext(tblcw);
    }
}
