////////////////////////////////////////////////////////
///////      Podstawowa konfiguracja
////////////////////////////////////////////////////////

//#define CPP_EXAMPLE

#define BUFFERSYNC

#define WIN_PLOT
#define NazwaPlikuDanych "Data/TMSLABoutputData.csv"
#define CSV_NAGLOWEK "Wsp. x,Wsp. y1,Wsp. y2\n"
#define CSV_DANE "%i,%i,%i\n", Tim, y[0], y[1]

////////////////////////////////////////////////////////

#include "main.h"
// MY
//#include <vector>
#include "vector.h"
#include <iostream>
#include "UAR.h"
#include "PID.h"
#include "InercModel.h"
#include <limits>
// MY

#ifdef TMSLAB_WIN
#include "stdio.h"
#endif

unsigned long *ekran; // Adres obszaru graficznego LCD [8*128*2]
#ifdef TMSLAB_C2000
unsigned char *textEkran; // Adres obszaru tekstowego [40*16/2]
#endif

#ifdef TMSLAB_WIN
unsigned short int *textEkran; // Adres obszaru tekstowego [40*16/2]
extern int (*PartialRefresh)();
char credits[43] = "-               DEMO DISC                -"; // Tekst wyswietlany w naglowku symulatora
long Timer2IsrPeriod = 1;                                        // okres pracy symulowanego licznika Timer2 podany w przybli�eniu w ms
#ifdef WIN_PLOT
FILE *outputCSV = 0;
#endif
#endif

int Tim = 0;                     // Licznik uzytkownika
unsigned int preScale = 0;       // Preskaler licznika uzytkownika
volatile char EnableRefresh = 0; // Zezwolenie na odswiezenie zawartosci pamieci graficznej

R_P_LCD_TMSLAB LCD;           // Obiekt obslugujacy LCD
R_P_KEYBOARD_TMSLAB KEYBOARD; // Obiekt obslugujacy klawiature
R_P_LEDBAR_TMSLAB LEDBAR;     // Obiekt obslugujacy diody LED

// Tablice danych/obiektow graficznych
#define MaxObj 200
#ifdef CPP_EXAMPLE
square objects[MaxObj];
#else
int dx[MaxObj];
int dy[MaxObj];
int s[MaxObj];
int x[MaxObj];
int y[MaxObj];
#endif

unsigned char Bufor[] = "KCode:  ";
const int X = 255;
const int Y = 127;

int main()

{
    SetUpPeripherials();
#ifdef TMSLAB_C2000
    LCD.LCD_Init(ekran, textEkran);
#endif

#ifdef TMSLAB_WIN
    LCD.LCD_Init(&ekran, &textEkran);
#ifdef WIN_PLOT
    outputCSV = fopen(NazwaPlikuDanych, "w+");
    fprintf(outputCSV, CSV_NAGLOWEK);
#endif
#endif

    KEYBOARD.InitKB(100);

    LEDBAR.InitLedBar();

    InitData();

    EnableInterrupts();

    // UAR
    float yZad = 0;
    float HReal = 100;
    int Tp = 1;

    // PID
    float kp = 0.1;
    float Ti = 1;
    float Td = 0.2;
    float Smin = 0;
    float Smax = HReal;
    PID pid = PID(kp, Ti, Td, Tp, Smin, Smax);
    UAR *C = &pid;

    // Object
    int pole = 0.1;
    InercModel im = InercModel(pole);
    UAR *G = &im;

    // Size
    int a = X / 2 - 20;
    int H = Y / 2 - 10;

    // Wykres1 - y(nTp)
    int x0_1 = 0;
    int y0_1 = 0;
    Vector v1(1);
    v1[0] = 0;

    // Wykres2 - e(nTp)
    int x0_2 = X / 2 + 5;
    int y0_2 = 0;
    Vector v2(1);
    v2[0] = 0;

    // Wykres3 - u(nTp)
    int x0_3 = 0;
    int y0_3 = Y / 2 + 5;
    Vector v3(1);
    v3[0];

    // WaterTank
    int x0_4 = X / 2 + 5;
    int y0_4 = Y / 2 + 5;

    // Time
    int TimPrev = 0;

    // Signals
    float e = 0;
    float u = 0;
    float yWy = 0;

    // Text
    std::string str = "";

    // Mode2 - Full Screen
    int a2 = X - 20;
    int H2 = Y - 10;
    int x0 = 0;
    int y0 = 0;
    int screenNumber = 1;

    // How to use:
    //	1 - Water
    //  2 - y
    //	3 - e
    //	4 - u
    //	5 - Everything
    //	6 - yZad = 0
    //	7 - yZad = 0.25H
    //	8 - yZad = 0.5H
    //	9 - yZad = 0.75H
    //	10 - yZad = H

    while (1)
    {
        EnableRefresh = 1;
        LCD.Synchronize();
        EnableRefresh = 0;
        ClearScreen();
        str = "      ";
        PrintText(textEkran, str, str.size(), 33, 0);
        PrintText(textEkran, str, str.size(), 14, 8);
        PrintText(textEkran, str, str.size(), 14, 0);

        unsigned char Key = KEYBOARD.GetKey();
        if (ButtonClicked(Key))
        {
            if (Key >= 6 && Key <= 10)
            {
                yZad = ConvertKeyToYZad(LastClickedKey(Key), HReal);
                yWy = 0;
                PID pid = PID(kp, Ti, Td, Tp, Smin, Smax);
                C = &pid;
                InercModel im = InercModel(pole);
                G = &im;
                v1.clear();
                v1.push_back(0);
                v2.clear();
                v2.push_back(0);
                v3.clear();
                v3.push_back(0);
            }
            if (Key >= 1 && Key <= 5)
            {
                screenNumber = Key;
            }
        }

        switch (screenNumber)
        {
        case 1:
            WaterTank(a2, H2, x0, y0, yWy, HReal);
            break;
        case 2:
            str = "y(nTp)";
            PrintText(textEkran, str, str.size(), 33, 0);
            FunctionRealTime(a2, H2, x0, y0, v1, HReal, false);
            FunctionRealTime(a2, H2, x0, y0, v2, HReal, false);
            FunctionRealTime(a2, H2, x0, y0, v3, HReal, true);
            break;
        case 3:
            str = "e(nTp)";
            PrintText(textEkran, str, str.size(), 33, 0);
            FunctionRealTime(a2, H2, x0, y0, v1, HReal, false);
            FunctionRealTime(a2, H2, x0, y0, v2, HReal, true);
            FunctionRealTime(a2, H2, x0, y0, v3, HReal, false);
            break;
        case 4:
            str = "u(nTp)";
            PrintText(textEkran, str, str.size(), 33, 0);
            FunctionRealTime(a2, H2, x0, y0, v1, HReal, true);
            FunctionRealTime(a2, H2, x0, y0, v2, HReal, false);
            FunctionRealTime(a2, H2, x0, y0, v3, HReal, false);
            break;
        case 5:
            str = "u(nTp)";
            PrintText(textEkran, str, str.size(), 14, 0);
            str = "e(nTp)";
            PrintText(textEkran, str, str.size(), 33, 0);
            str = "y(nTp)";
            PrintText(textEkran, str, str.size(), 14, 8);

            FunctionRealTime(a, H, x0_1, y0_1, v1, HReal, true);
            FunctionRealTime(a, H, x0_2, y0_2, v2, HReal, true);
            FunctionRealTime(a, H, x0_3, y0_3, v3, HReal, true);
            WaterTank(a, H, x0_4, y0_4, yWy, HReal);
            Cross();
            break;
        }

        if (Tim != TimPrev)
        {
            if (TimPrev % Tp == 0)
            {
                e = yZad - yWy;
                C->setInput(e);
                C->Calculate();

                u = C->getOutput();
                G->setInput(u);
                G->Calculate();

                yWy = G->getOutput();
                std::cout << "y = " << yWy << ", yZad =  " << yZad << ", yMax = " << HReal << std::endl;

                v1.push_back(u);
                v2.push_back(e);
                v3.push_back(yWy);
            }
            TimPrev = Tim;
        }

#ifdef TMSLAB_WIN
        if (PartialRefresh())
            return 0;
#ifdef WIN_PLOT
        // Zapis danych do pliku
        fprintf(outputCSV, CSV_DANE);
        // printf("time %i \n", Tim);
        fflush(outputCSV);
        fflush(stdout);
#endif
#endif
    }
}

float ConvertKeyToYZad(int Key, float HReal)
{
    switch (Key)
    {
    case 6:
        return 0;
        break;
    case 7:
        return HReal * 0.25;
        break;
    case 8:
        return HReal * 0.5;
        break;
    case 9:
        return HReal * 0.75;
        break;
    case 10:
        return HReal * 1;
        break;
    }
    return 0;
}

bool ButtonClicked(int Key)
{
    static bool block = false;
    if (Key != 0 && block == false)
    {
        block = true;
        return true;
    }
    if (Key == 0)
    {
        block = false;
    }
    return false;
}

int LastClickedKey(int Key)
{
    static unsigned char KeyCurrent = 0;
    // printf("%i \n", Key);
    if (Key != 0)
    {
        KeyCurrent = Key;
    }
    return KeyCurrent;
}

void FunctionRealTime(int a, int H, int x0, int y0, Vector &v, float HReal, bool visualize)
{

    if (visualize)
    {
        DrawAxes(a, H, x0, y0);
    }
    int n = 20;
    int dx = a / n;
    int x = x0;
    int y = 0;
    for (int i = 0; i < v.size(); i++)
    {
        y = v[i];
        y = y * H / HReal;
        y = H + y0 - y;
        if (visualize)
        {
            SetPixel(ekran, x, y);
        }
        if (x <= a + x0)
        {
            x = x + dx;
        }
        else
        {
            v.erase(v.begin());
        }
    }
}

void DrawAxes(int a, int H, int x0, int y0)
{
    for (int y = y0; y <= y0 + H; y++)
    {
        for (int x = x0; x <= x0 + a; x++)
        {
            if (y == y0 + H || x == x0)
            {
                SetPixel(ekran, x, y);
            }
        }
    }
}

void Cross()
{
    for (int y = 0; y < Y; y++)
    {
        for (int x = 0; x < X; x++)
        {
            if (y == Y / 2 || x == X / 2)
            {
                SetPixel(ekran, x, y);
            }
        }
    }
}

void WaterTank(int a, int H, int x0, int y0, float hReal, float HReal)
{
    float h = hReal * H / HReal;
    for (int y = y0; y <= y0 + H; y++)
    {
        if (y == y0 + H)
        {
            for (int x = x0; x <= x0 + a; x++)
            {
                SetPixel(ekran, x, y);
            }
        }
        for (int x = x0; x <= x0 + a; x++)
        {
            if ((x == x0) || (x == x0 + a))
                SetPixel(ekran, x, y);
        }
    }
    for (int y = y0; y <= y0 + H; y++)
    {
        if (y <= y0 + H && y >= y0 + H - h)
        {
            for (int x = x0; x <= x0 + a; x++)
            {
                SetPixel(ekran, x, y);
            }
        }
    }
}

#ifdef TMSLAB_C2000

interrupt void Timer2Isr()
{

#ifdef BUFFERSYNC
    if (EnableRefresh)
        LCD.PartialRefresh();
#else
    LCD.PartialRefresh();
#endif

    KEYBOARD.PartialRefresh();

    if (++preScale == 50000)
    {
        preScale = 0;
        Tim++;
    }
}

unsigned long ADRFTECHED = 0;
interrupt void NoIsr()
{
    ADRFTECHED = PieCtrlRegs.PIECTRL.bit.PIEVECT;
    asm(" ESTOP0");
}

void EnableInterrupts()
{

    EALLOW;
    // Ustawienie wektorow przerwan
    unsigned long VECTBEG = (unsigned long)&PieVectTable;
    unsigned long VECTLAST = (unsigned long)&PieVectTable + sizeof(PieVectTable);
    while (VECTBEG >= VECTLAST)
        *(unsigned long *)VECTBEG++ = (unsigned long)NoIsr;
    PieVectTable.TIMER2_INT = Timer2Isr;

    CpuTimer2Regs.TCR.bit.TIE = 1;
    CpuTimer2Regs.TCR.bit.TRB = 1;

    IER = IER_MASK; // Odblokuj przerwania
    asm(" push ier");
    asm(" pop dbgier");

    PieCtrlRegs.PIECTRL.bit.ENPIE = 1;
    PieCtrlRegs.PIEACK.all = 0xffff;
    EDIS;
    EINT;
}

void SetUpPeripherials()
{
    SetupCoreSystem();
    ekran = (unsigned long *)0x8000;     //[8*128*2]
    textEkran = (unsigned char *)0x8a00; //[40*16/2]
    EALLOW;
    // Okres licznika T2
    CpuTimer2Regs.PRD.all = System_Clk * Timer2ISR_Period;
    EDIS;
}
extern "C"
{
    int _system_pre_init()
    {
        EALLOW;
        WdRegs.WDWCR.all = 0x68;
        EDIS;
        return (1);
    }
}
#endif

#ifdef TMSLAB_WIN
void EnableInterrupts()
{
}
void SetUpPeripherials()
{
}
void Timer2Isr()
{
    if (++preScale == 500)
    {
        preScale = 0;
        Tim++;
    }
}
#endif

void InitData()
{
    for (int a = 0; a < (128 * 8); a++)
        ekran[a] = 0;
    for (int a = 0; a < (40 * 16); a++)
        textEkran[a] = ' ';
#ifndef CPP_EXAMPLE
    for (int a = 0; a < MaxObj; a++)
    {
        s[a] = (rand() & 0x1f) + 1;
        x[a] = rand() & 0x03ff;
        if (x[a] > 239 * 4 - s[a])
            x[a] -= 239 * 4 - s[a];
        y[a] = rand() & 0x1ff;
        if (y[a] > 127 * 4 - s[a])
            y[a] -= 127 * 4 - s[a];
        dx[a] = ((rand() & 0x1) ? 1 : -1) * (((rand() & 0x7) + 2) >> 1);
        dy[a] = ((rand() & 0x1) ? 1 : -1) * (((rand() & 0x7) + 2) >> 1);
    }
#endif
}
void ClearScreen()
{

    for (int a = 0; a < (128 * 8); a++)
        ekran[a] = 0;
}

void DrawPixels(int Key)
{
#ifdef CPP_EXAMPLE
    for (int c = 0; c < MaxObj; c++)
    {
        objects[c].move(Key, Key);
        objects[c].draw();
    }
#else
    for (int c = 0; c < MaxObj; c++)
    {
        y[c] += dy[c] + 6 - Key;
        x[c] += dx[c] + 6 - Key;
        if (x[c] < 0)
            x[c] += 239 * 4 - s[c];
        if (y[c] < 0)
            y[c] += 127 * 4 - s[c];
        if (x[c] > 239 * 4 - s[c])
            x[c] -= 239 * 4 - s[c];
        if (y[c] > 127 * 4 - s[c])
            y[c] -= 127 * 4 - s[c];
        long sdec = (((x[c] - 120 * 4L) * (x[c] - 120 * 4L)) >> 13) + (((y[c] - 58 * 4L) * (y[c] - 58 * 4L)) >> 9);
        long size = s[c] - sdec;
        if (size < 0)
            size = 0;
        size = s[c] - size;

        for (int b = y[c] >> 2; b < (y[c] + size) >> 2; b++)
            for (int a = x[c] >> 2; a < (x[c] + size) >> 2; a++)
                SetPixel(ekran, a, b);
    }
#endif
}
