////////////////////////////////////////////////////////
///////      Podstawowa konfiguracja
////////////////////////////////////////////////////////

//#define CPP_EXAMPLE

#define BUFFERSYNC

#define WIN_PLOT
#define NazwaPlikuDanych "Data/TMSLABoutputData.csv"
#define CSV_NAGLOWEK "Wsp. x,Wsp. y1,Wsp. y2\n"
#define CSV_DANE "%i,%i,%i\n",Tim, y[0], y[1]

////////////////////////////////////////////////////////


#include "main.h"

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
char credits[43]="-               DEMO DISC                -";  // Tekst wyswietlany w naglowku symulatora
long Timer2IsrPeriod=1; // okres pracy symulowanego licznika Timer2 podany w przybli�eniu w ms
#ifdef WIN_PLOT
  FILE* outputCSV=0;
#endif
  #endif

int Tim = 0;                // Licznik uzytkownika
unsigned int preScale = 0;  // Preskaler licznika uzytkownika
volatile char EnableRefresh = 0;    //Zezwolenie na odswiezenie zawartosci pamieci graficznej

R_P_LCD_TMSLAB LCD;             // Obiekt obslugujacy LCD
R_P_KEYBOARD_TMSLAB KEYBOARD;   // Obiekt obslugujacy klawiature
R_P_LEDBAR_TMSLAB LEDBAR;       // Obiekt obslugujacy diody LED


//Tablice danych/obiektow graficznych
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

    int main()

    {
        SetUpPeripherials();
#ifdef TMSLAB_C2000
        LCD.LCD_Init(ekran, textEkran);
#endif

#ifdef TMSLAB_WIN
        LCD.LCD_Init(&ekran,&textEkran);
#ifdef WIN_PLOT
  outputCSV=fopen(NazwaPlikuDanych,"w+" );
  fprintf(outputCSV,CSV_NAGLOWEK);
#endif
#endif

        KEYBOARD.InitKB(100);

        LEDBAR.InitLedBar();

        InitData();

        EnableInterrupts();

        while (1)
        {
            EnableRefresh = 1;
            LCD.Synchronize();
            EnableRefresh = 0;

            unsigned char Key = KEYBOARD.GetKey();

            LEDBAR.SetValue(Tim);

            Bufor[6] = Key / 10 + '0';
            Bufor[7] = Key % 10 + '0';
            PrintText(textEkran, Bufor, 8, 16, 7);
            ClearScreen();
            DrawPixels(Key);
#ifdef TMSLAB_WIN
            if(PartialRefresh()) return 0;
#ifdef WIN_PLOT
	// Zapis danych do pliku
	fprintf(outputCSV,CSV_DANE);
	printf("time %i \n",Tim);
	fflush(outputCSV);
	fflush(stdout);
#endif
#endif

        }
    }

#ifdef TMSLAB_C2000

    interrupt
    void Timer2Isr()
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
    interrupt
    void NoIsr()
    {
        ADRFTECHED = PieCtrlRegs.PIECTRL.bit.PIEVECT;
        asm(" ESTOP0");
    }

    void EnableInterrupts()
    {

        EALLOW;
        //Ustawienie wektorow przerwan
        unsigned long VECTBEG = (unsigned long) &PieVectTable;
        unsigned long VECTLAST = (unsigned long) &PieVectTable
        + sizeof(PieVectTable);
        while (VECTBEG >= VECTLAST)
        *(unsigned long*) VECTBEG++ = (unsigned long) NoIsr;
        PieVectTable.TIMER2_INT = Timer2Isr;

        CpuTimer2Regs.TCR.bit.TIE = 1;
        CpuTimer2Regs.TCR.bit.TRB = 1;

        IER = IER_MASK;//Odblokuj przerwania
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
        ekran = (unsigned long *) 0x8000;              //[8*128*2]
        textEkran = (unsigned char*) 0x8a00;//[40*16/2]
        EALLOW;
        //Okres licznika T2
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
    {}
    void SetUpPeripherials()
    {}
    void Timer2Isr()
    {
        if(++preScale==500)
        {   preScale=0;Tim++;}
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
        for (int c=0;c<MaxObj;c++)
        {
            objects[c].move(Key,Key);
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
            long sdec = (((x[c] - 120 * 4L) * (x[c] - 120 * 4L)) >> 13)
                    + (((y[c] - 58 * 4L) * (y[c] - 58 * 4L)) >> 9);
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
