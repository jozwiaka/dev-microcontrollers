
#ifndef main_CL
#define main_CL


#include "setup.h"
#include "stdlib.h"
#include "R_P_LCD_TMSLAB.h"
#include "R_P_KEYBOARD_TMSLAB.h"
#include "R_P_LEDBAR_TMSLAB.h"
#include "square.h"

#ifdef TMSLAB_C2000
#include "F2837xD_device.h"
#include "systemInit.h"
#endif


void SetUpPeripherials();
void EnableInterrupts();
void InitData();
void ClearScreen();
void DrawPixels(int Key);

void ClearText();
bool ButtonClicked(int);
int LastClickedKey(int);
void FunctionRealTime(int, int, int, int, int*, float);
void DrawAxes(int, int, int, int);
void Cross();
void WaterTank_Pipes(int, int, int, int, float, float, float, float);
void WaterTank(int, int, int, int, float);
int Pipe_In(int, int, int, int, float, int);
void Pipe_Out(int, int, int, int, float, float, float);
void WaterAnimation(int, int, int, int, float, int, int);
float PID_Calculate(float, bool);
float InercModel_Calculate(float);
void clearArray(int*, int);



#endif
