
#ifndef main_CL
#define main_CL


#include "setup.h"
#include "stdlib.h"
#include "R_P_LCD_TMSLAB.h"
#include "R_P_KEYBOARD_TMSLAB.h"
#include "R_P_LEDBAR_TMSLAB.h"
#include "square.h"

#include "vector.h"

#ifdef TMSLAB_C2000
#include "F2837xD_device.h"
#include "systemInit.h"
#endif


void SetUpPeripherials();
void EnableInterrupts();
void InitData();
void ClearScreen();
void DrawPixels(int Key);


float ConvertKeyToYZad(int Key, float HReal);
bool ButtonClicked(int Key);
int LastClickedKey(int Key);
void FunctionRealTime(int a, int H, int x0, int y0, Vector& v, float HReal, bool visualize);
void DrawAxes(int a, int H, int x0, int y0);
void WaterTank(int a, int H, int x0, int y0, float hReal, float HReal);
void Cross();

#endif
