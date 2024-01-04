//////////////////////////////////////////////////////////////////////////////////////////
//
// Projekt przykladowy dla symulator MKD51 SiM  w wersji od 0.17
//
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////////////
// ***************  Elementy niezgodnosci sprzetowej symulatora z urzadzeniem MKD51:
// P1.2-P1.5 znajduja sie w symulatorze pod wylaczna kontrola klawiatury matrycowej w trakcie ich odczytu (na chwile obecna, nie ma potrzeby ustwiania tych bitow na jedynki, co jest konieczne w ukladzie rzeczywistym)
// Wyswietlacz LCD nie obsluguje tworzenia wlasnego zestawu znakow.
// Stan wszystkich LED-ow i wyswietlaczy 7-segm. jest symulowany z okresem 50us i aktualizowany na monitorze z okresem 25ms dla ledow oraz okresem w zakresie od 50ms do 250 ms (regulowanym potencjometrem "jasnosc") dla modulow 7-seg.
// Wyswietlacz 7-segmentowy symulowany jest w dwoch trybach:
// - dla nie zdefiniowanego symbolu Persistent_7sLED jest tylko jeden rejestr sterujacy katodami wszystkich modulow 7-segmentowych, bity P1.0 i P1.1 wybieraja numer modulu 7-seg. do zasilenia.
// - dla zdefiniowanego symbolu Persistent_7sLED, kazdy modul 7-seg posiada swoj indywidualny zatrzask (wybierany bitami P1.0 i P1.1) sterujacy segmentami, wszystkie moduly 7-seg. sa zalaczone.
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////////////
// ***************  Klawiszologia:
// Klawisze od 0 do 5 -> X0:X5
// Klawiatura numeryczna -> klawiatura matrycowa MKD51
// Klawiatrura numeryczna + CTRL -> nastawianie pozycji potencjometrow POT0-POT2
// LMB i ruch mysza na potencjometrze -> zgrubne nastawianie pozycji potencjometru
// LMB+RMB i ruch mysza na potencjometrze -> precyzyjne nastawianie pozycji potencjometru
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------

//Deklaracje rejestrow MCS-51
#include <reg52.h>
//Deklaracje funkcji wbudowanych wspieracjacych sprzetowe operacje MCS-51
#include <intrins.h>
//Dekaracje procedur obslugi LCD
#include "lcd.h"
// Konfiguracja parametrow symulatora
#include "mkd51sim.h"

//--------------------------------------------
// Deklaracje funkcji zaimplementowanych w asemblerze
#define MixedAsm

#ifdef MixedAsm
void INWERSJAP1_6();
void WAIT_10US(int wait);
unsigned char LONG_PARAM_LIST(long l1, long l2);
#endif
//--------------------------------------------

// Deklaracje rejestrow sprzetowych MKD51
unsigned char xdata PTWE _at_ 0x8008;
unsigned char xdata PTWY _at_ 0x8009;
unsigned char xdata PTSEG _at_ 0x8018;
unsigned char xdata POT0 _at_ 0x8005;

//Deklaracja rejestru obslugujacego wyswetlacz graficzny LCD symulatora
unsigned char xdata LCDWRITE _at_ 0x7fff;
#define CLEAR_GRAPH_MEMORY 2
#define COPY_SCREEN 1

//Deklaracja zmiannych jednobitowych mapowanych pod wskazanymi bitami
sbit P1_6 = P1 ^ 6; // Mapowowanie 6-tego bitu portu P1
sbit P1_7 = P1 ^ 7; // Mapowowanie 7-tego bitu portu P1

//Deklaracja lancucha tekstowego przeznaczonego do wyslania do drivera LCD
unsigned char xdata tblcw[] = "| OTO PIERWSZY |0    PROGRAM   0";

//Deklaracje zmiennych pomocniczych
unsigned char presc = 0;	 // Licznik preskalera czestotliwosci wywolywania podalgorytmu w przerwaniu
unsigned char Modul7Seg = 0; // Licznik kontrolujacy multipleksowanie wyswietlaczy 7-seg
unsigned char soundtim = 0;	 // Licznik opoznienia czasowgo wylaczenia buzera
bit flag = 0;				 // Flaga synchornizacji programu glownego z przerwaniem czasowym

data int ZEZWOLENIE_L8 = 1; // Zmienna udostepniana w procedurze napisanej w asemblerze

//Zmienne algorytmu rysujacego kwadrat na wyswietlaczu graficznym symulatora
unsigned int x = 30, y = 80; // Wspolrzedne polozenia kwadratu
unsigned int xx, yy;		 // Wspolrzedne generowanych pikseli
char dx = 1, dy = -1;		 // Skladowe wektora predkosci kwadratu
unsigned char xdata *pixel;	 // Wskaznik do danych piksela

//////////////////////////////////////////////////////////////////////////////////////////

void main(void)
{

//--------------------------------------------
//Wywolanie funkcji i procedury zdefiniowanej w asemblerze
#ifdef MixedAsm
	PTWY = LONG_PARAM_LIST(0, 0x81);
	WAIT_10US(-50000);
#endif
	//--------------------------------------------

	// Konfiguracja peryferiow MCS-51
	P1 = 0x3c;			  // Ustwienie bitow od 2 do 5 do pracy wejsciowej, wylacznie L8 i buzzera, wybranie modulu W0 7-seg
	TMOD = 1;			  // Ustawienie licznika 0 do pracy w trybie 1 i zliczania wewnetrznych impulsow zegarowych o okresie rownym 1us
						  // Inicjalizacja stanu poczatkowego licznika 0 (-2500 <=> okres do przepelnienia licznika = 2500us)
	TH0 = (-2500) >> 8;	  // Starszy bajt zwiazany z liczba -2500 zapisany w kodzie U2
	TL0 = (-2500) & 0xff; // Mlodszy bajt zwiazany z liczba -2500 zapisany w kodzie U2
	IE = 0x82;			  // Odblokowanie przerwan poprzez odblokowanie przerwania od licznika 0 bitem ET0 oraz ustawienie globalnego zezwolenia na wykonywanie przerwan bitem EA
	TCON = 0x10;		  // Uruchomienie licznika poprzez ustawienie bitu TR0
						  // Konfiguracja wyswietalcza alfanumerycznego LCD
	prglcd();
	//Pierwsza inicjalizacja pomiaru ADC na kanale 5 - POT0
	POT0 = 0;

	/////////////////////////////////////////////////////////////////////////////////////////////////////
	// Glowna petla programu
	/////////////////////////////////////////////////////////////////////////////////////////////////////

	while (1)
	{

//--------------------------------------------
// Programowe opoznienia czasowe
#ifdef MixedAsm
		WAIT_10US(-100);
#else
		unsigned int delay;
		for (delay = 0; delay < 200; delay++)
			_nop_();
#endif
		//--------------------------------------------

		//Wyslanie lancucha tekstowego na LCD
		disptext(tblcw);

		// Obsluga wyseiwtalacza graficznego LCD symulatora, synchronizowana flaga ustawiona w przerwaniu czasowym
		if (flag)
		{
			//Skasowanie flagi
			flag = 0;
			//Aktualizacje na LCD polozenia kwadratu kolorowego
			x += dx;
			y += dy;
			if (y == 0 || y == 107)
				dy = -dy;
			if (x == 0 || x == 43)
				dx = -dx;
			LCDWRITE = CLEAR_GRAPH_MEMORY;
			//Rysowanie w buforze LCD kwadratu wypelnionego kolorowym gradientem
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
			//Kopiowanie bufora LCD na ekran
			LCDWRITE = COPY_SCREEN;
		}
	}
}
//////////////////////////////////////////////
//interrupt 0 <=> EXTERNAL INT 	0 	0003h
//interrupt 1 <=> TIMER/COUNTER 0	 	000Bh
//interrupt 2 <=> EXTERNAL INT 1		0013h
//interrupt 3 <=> TIMER/COUNTER 1		001Bh
//interrupt 4 <=> SERIAL PORT				0023h
//interrupt 5 <=> TIMER/COUNTER 2 	002Bh

//Procedura obslugi przerwania od licznika programowalnego 0
void IRT_TIMER0() interrupt 1
{
	//Reinicjalizacja stanu poczatkowego licznika 0 (-2500 <=> okres do przpelnienia licznika = 2500us)
	TH0 = (-2500) >> 8;	  // Starszy bajt zwiazany z liczba -2500 zapisany w kodzie U2
	TL0 = (-2500) & 0xff; // Mlodszy bajt zwiazany z liczba -2500 zapisany w kodzie U2

	flag = 1; // Informacja zwrotna do programu glownego o uplywie czasu 2500us

	//Preskaler czestotliwosci wywolywania podalgorytmu w przerwaniu, wymuszajacy jego prace z okresm 200*2,5ms=500ms
	if (++presc == 200)
	{
		presc = 0;

//--------------------------------------------
// Inwersja diody L8
#ifdef MixedAsm
		INWERSJAP1_6();
#else
		P1_6 = !P1_6;
#endif
		//--------------------------------------------
		// Dyanmiczna modyfikacja lancucha tekstowego
		tblcw[16]++;
		tblcw[16] &= 48 + 7;
		tblcw[31] = tblcw[16];
		//Zalaczenie buzzera
		P1_7 = 1;
		soundtim = 30;
	}
	//Wylaczenie buzzera z opoznieniem czasowym
	if (soundtim > 0)
		soundtim--;
	else
		P1_7 = 0;

//--------------------------------------------
//Alternatywny tryb pracy wysweitlaczy LED-owych w symulatorze z indywidualnymi zatrzaskami dla kazdego modulu 7-seg
#ifndef Persistent_7sLED
	PTSEG = 0;
#endif
	//--------------------------------------------
	//Implementacja licznika modulo 4 sluzacego do multipleksowania wyswietlaczy 7-seg
	Modul7Seg = ++Modul7Seg & 3;
	//Wygaszenie segmentow przed zmiana zailania modulu 7-seg
	PTSEG = 0;
	//Wysterowanie wybranego modulu 7-seg - multipleskowanie
	P1 = (P1 & 0xfc) | Modul7Seg | 0x3c;
	//Wysterowanie segmentow wyswietlacza 7-seg informacja zbiorcza zwiazana ze stanem multipleksu i przyciskami MKD51
	PTSEG = Modul7Seg | (~P1 & 0x7c) | ((~PTWE << (7 - Modul7Seg)) & 0x80);
	//Wysterowanie diod L7-L0 stanem konwersji ADC, zainicjalizowanym przy poprzednim wywolaniu przerwania
	PTWY = POT0;
	//Inicjalizacja kolejnego pomiaru ADC na kanale 5 - POT0
	POT0 = 0;
}
