
/////////////////////////////////////////////
// - Ustawienia symulatora MKD51
//#define Persistent_7sLED
#define POT0_START_POS 0 
#define POT1_START_POS 128
#define POT2_START_POS 255
#define POTL_START_POS 128
/////////////////////////////////////////////

//-------------------------------------------

/////////////////////////////////////////////
// Konfiguracja komunikacji z symulatorem
#ifdef Persistent_7sLED
code unsigned char Persistend7sLED _at_ 0x1001;
#else
code unsigned char Persistend7sLED _at_ 0x1000;
#endif
code unsigned char POT0_START _at_ 0x2000+POT0_START_POS;
code unsigned char POT1_START _at_ 0x2100+POT1_START_POS;
code unsigned char POT2_START _at_ 0x2200+POT2_START_POS;
code unsigned char POT7LED_START _at_ 0x2300+POTL_START_POS; 
//////////////////////////////////////////////
