#include <reg52.h>
#include <intrins.h>
#include "lcd.h"
#include "mkd51sim.h"

unsigned char xdata PTWE _at_ 0x8008;
unsigned char xdata PTWY _at_ 0x8009;
unsigned char xdata tblcw[] = "00.00";
float fNumber = 0;
//2500*200*2=1000000 - 1s
int counter = 0;
char restart = 0;
char restarted = 0;
const int time = -1000; 
void main(void)
{
    prglcd();

    TMOD = 1;
    TH0 = time >> 8;
    TL0 = time & 0xff;
    TR0 = 0;
    while (1)
    {
        if (!(PTWE & 0x01))
        {
            TR0 = 1;
        }
        if (!(PTWE & 0x02))
        {
            TR0 = 0;
        }
        if (!(PTWE & 0x04))
        {
            if (!restarted)
            {
                TR0 = 1;
                restarted = 1;
                restart = 1;
            }
        }
        else
        {
            restarted = 0;
        }
        if (TF0)
        {
            TF0 = 0;
            TH0 = time >> 8;
            TL0 = time & 0xff;
         counter++;
        }
        if (restart)
         counter = 0;
        restart = 0;
        fNumber = (float) counter / 1000; //1.234
        tblcw[0] = '0' + fNumber;
        fNumber = (fNumber - (int)fNumber) * 10;
        tblcw[1] = '0' + fNumber;
        tblcw[2] = '.';
        fNumber = (fNumber - (int)fNumber) * 10;
        tblcw[3] = '0' + fNumber;
        fNumber = (fNumber - (int)fNumber) * 10;
        tblcw[4] = '0' + fNumber;
        disptext(tblcw);
    }
}
