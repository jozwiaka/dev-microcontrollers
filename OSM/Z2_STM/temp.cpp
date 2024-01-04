#include "main.h"
// MY
#include "stdio.h"
// MY

int Tim = 0; // Licznik uzytkownika
int TimPrev = 0;
unsigned int preScale = 0;       // Preskaler licznika uzytkownika
volatile char EnableRefresh = 0; // Zezwolenie na odswiezenie zawartosci pamieci graficznej

// Screen
const int X = 255;
const int Y = 127;
const int a[] = {X / 2 - 20, X - 20};
const int H[] = {Y / 2 - 10, Y - 10};
const int x0[] = {0, X / 2 + 5};
const int y0[] = {0, Y / 2 + 5};
int screenNumber = 1;

// Tp
const unsigned int Tp_ms = 500;
const float Tp = (float)Tp_ms / 1000;

// UAR
const float H_MAX = 100;
float yZad = H_MAX / 2;

// PID
float kp = 0.1;
float Ti = 1;
float Td = 0.2;
float ki = kp * Tp / Ti;
float kd = kp * Td / Tp;
const float Smin = 0;
const float Smax = H_MAX;

// G
float pole = 0.1;

// Signals
float e = 0;
float u = 0;
float yWy = 0;
float yWyPrev = 0;
float z = 10;

// v
const int nv = 40;
int iv = 0;
int v_u[nv] = {};
int v_e[nv] = {};
int v_yWy[nv] = {};

bool block = false;

unsigned char ch_u[] = "u(n)";
unsigned char ch_e[] = "e(n)";
unsigned char ch_y[] = "y(n)";

int main()
{
    while (1)
    {
        TFTDisplay_ILI9341_Initialization(240, 320);
        TFTDisplay_ILI9341_FillScreen(TFT_COLOR_ILI9341_WHITE);
        osDelay(1000);
        // 2. GET AND PROCESS AN INPUT; CLEAR DATA IF SCREEN 3
        unsigned char Key = KEYBOARD.GetKey();

        if (GPIOA->IDR & 0x0001)
        {
            if (!block)
            {
                osDelay(2);
                screenNumber++;
                if (screenNumber == 3)
                {
                    yWy = 0;
                    yWyPrev = 0;
                    iv = 0;
                    screenNumber = 1;
                    PID_Calculate(0, true);
                    clearArray(v_u, nv);
                    clearArray(v_e, nv);
                    clearArray(v_yWy, nv);
                }
                block = true;
            }
        }
        else
        {
            block = false;
        }
    }

    switch (screenNumber)
    {
    case 1:
        WaterTank_Pipes(a[1], H[1], x0[0], y0[0], yWy, yWyPrev, H_MAX, z);
        break;

    case 2:
        FunctionRealTime(a[0], H[0], x0[0], y0[0], v_u, H_MAX);
        FunctionRealTime(a[0], H[0], x0[1], y0[0], v_e, H_MAX);
        FunctionRealTime(a[0], H[0], x0[0], y0[1], v_yWy, H_MAX);
        WaterTank_Pipes(a[0], H[0], x0[1], y0[1], yWy, yWyPrev, H_MAX, z);
        Cross();

        ptr = ch_e;
        nx = 0;
        ny = 0;
        for (int i = 0; i < 9; i++)
        {
            TFTDisplay_ILI9341_DrawChar(nx, ny, *ptr);
            nx = nx + 10;
            ptr++;
        }
        ptr = ch_u;
        nx = 0;
        ny = 0;
        for (int i = 0; i < 9; i++)
        {
            TFTDisplay_ILI9341_DrawChar(nx, ny, *ptr);
            nx = nx + 10;
            ptr++;
        }
        ptr = ch_y;
        nx = 0;
        ny = 0;
        for (int i = 0; i < 9; i++)
        {
            TFTDisplay_ILI9341_DrawChar(nx, ny, *ptr);
            nx = nx + 10;
            ptr++;
        }
        break;
    }

    if (Tim != TimPrev)
    {
        e = yZad - yWy;
        u = PID_Calculate(e, false);
        yWyPrev = yWy;
        yWy = InercModel_Calculate(u) - z;
        if (yWy < 0)
        {
            yWy = 0;
        }
        if (iv == nv - 1)
        {
            for (int i = 0; i < nv - 1; i++)
            {
                v_u[i] = v_u[i + 1];
                v_e[i] = v_e[i + 1];
                v_yWy[i] = v_yWy[i + 1];
            }
            v_u[iv] = u;
            v_e[iv] = e;
            v_yWy[iv] = yWy;
        }
        else
        {
            v_u[iv] = u;
            v_e[iv] = e;
            v_yWy[iv] = yWy;
            iv++;
        }
        TimPrev = Tim;
    }
}

void clearArray(int *ar, int n)
{
    for (int i = 0; i < n; i++)
    {
        ar[i] = 0;
    }
}

float PID_Calculate(float in, bool clear)
{
    static float sum = 0;
    static float inPrev = 0;
    float out;
    if (clear)
    {
        inPrev = 0;
        sum = 0;
        return 0;
    }
    sum += in * ki;
    if (sum > Smax)
        sum = Smax;
    if (sum < Smin)
        sum = Smin;
    out = sum + in * kp + (in - inPrev) * kd;
    inPrev = in;
    return out;
}

float InercModel_Calculate(float in)
{
    float out = 0;
    out = out * pole + (1 - pole) * in;
    return out;
}

void FunctionRealTime(int a, int H, int x0, int y0, int *v, float H_MAX)
{
    DrawAxes(a, H, x0, y0);
    int dx = a / nv;
    int x = x0;
    int y = 0;
    for (int i = 0; i < nv; i++)
    {
        if (v[i] != 0)
        {
            y = v[i];
            y = y * H / H_MAX;
            y = H + y0 - y;
            TFTDisplay_ILI9341_DrawPixel(x, y, TFT_COLOR_ILI9341_BLACK);
        }
        x = x + dx;
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
                TFTDisplay_ILI9341_DrawPixel(x, y, TFT_COLOR_ILI9341_BLACK);
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
                TFTDisplay_ILI9341_DrawPixel(x, y, TFT_COLOR_ILI9341_BLACK);
            }
        }
    }
}

void WaterTank_Pipes(int a, int H, int x0, int y0, float yWy, float yWyPrev, float H_MAX, float z)
{
    int a_P = a / 4;
    int a_WT = a - 2 * a_P;

    int H_WT = H;
    int H_P = H / 10;
    int H_WA = H - H_P;

    int x0_WT = x0 + a_P;
    int y0_WT = y0;
    int x0_P1 = x0;
    int y0_P1 = y0;
    int x0_P2 = x0 + a_P + a_WT;
    int y0_P2 = y0 + H - H_P;
    int x0_WA = x0 + a_P;
    int y0_WA = y0 + H_P;

    float h_WT = yWy * H / H_MAX;
    int h_P1 = 0;
    float h_P2 = z * H / H_MAX;

    float dh_WT = (float)(yWy - yWyPrev) * H / H_MAX;

    WaterTank(a_WT, H_WT, x0_WT, y0_WT, h_WT);
    h_P1 = Pipe_In(a_P, H_P, x0_P1, y0_P1, dh_WT, H_WT);
    WaterAnimation(a_WT, H_WA, x0_WA, y0_WA, dh_WT, H_WT, h_P1);
    Pipe_Out(a_P, H_P, x0_P2, y0_P2, h_P2, h_WT, H_MAX);
}

void WaterAnimation(int a, int H, int x0, int y0, float dh_WT, int H_WT, int h_P1)
{
    const float vMax = (float)H / (a * a);
    float v;
    if (dh_WT > 0 && dh_WT <= H_WT / 100)
    {
        v = 50 * vMax;
    }
    if (dh_WT > H_WT / 100 && dh_WT <= H_WT / 90)
    {
        v = 45 * vMax;
    }
    if (dh_WT > H_WT / 90 && dh_WT <= H_WT / 80)
    {
        v = 40 * vMax;
    }
    if (dh_WT > H_WT / 80 && dh_WT <= H_WT / 70)
    {
        v = 35 * vMax;
    }
    if (dh_WT > H_WT / 70 && dh_WT <= H_WT / 60)
    {
        v = 30 * vMax;
    }
    if (dh_WT > H_WT / 60 && dh_WT <= H_WT / 50)
    {
        v = 25 * vMax;
    }
    if (dh_WT > H_WT / 50 && dh_WT <= H_WT / 40)
    {
        v = 20 * vMax;
    }
    if (dh_WT > H_WT / 40 && dh_WT <= H_WT / 30)
    {
        v = 15 * vMax;
    }
    if (dh_WT > H_WT / 30 && dh_WT <= H_WT / 20)
    {
        v = 10 * vMax;
    }
    if (dh_WT > H_WT / 20 && dh_WT <= H_WT / 10)
    {
        v = 5 * vMax;
    }
    if (dh_WT > H_WT / 10 && dh_WT <= H_WT)
    {
        v = 1 * vMax;
    }

    int y;
    if (dh_WT > 0)
    {
        for (int x = x0; x <= x0 + a; x++)
        {
            for (int i = 0; i <= h_P1; i++)
            {
                y = y0 + v * (x - x0) * (x - x0) - i;
                if (y < y0 + H && x < x0 + a)
                    TFTDisplay_ILI9341_DrawPixel(x, y, TFT_COLOR_ILI9341_BLUE);
            }
        }
    }
}

void WaterTank(int a, int H, int x0, int y0, float h)
{
    for (int y = y0; y <= y0 + H; y++)
    {
        if (y == y0 + H)
        {
            for (int x = x0; x <= x0 + a; x++)
            {
                TFTDisplay_ILI9341_DrawPixel(x, y, TFT_COLOR_ILI9341_BLACK);
            }
        }
        for (int x = x0; x <= x0 + a; x++)
        {
            if ((x == x0) || (x == x0 + a))
                TFTDisplay_ILI9341_DrawPixel(x, y, TFT_COLOR_ILI9341_BLACK);
        }
    }

    for (int y = y0; y <= y0 + H; y++)
    {
        if (y <= y0 + H && y >= y0 + H - h)
        {
            for (int x = x0; x <= x0 + a; x++)
            {
                TFTDisplay_ILI9341_DrawPixel(x, y, TFT_COLOR_ILI9341_BLUE);
            }
        }
    }
}

int Pipe_In(int a, int H, int x0, int y0, float dh_WT, int H_WT)
{
    for (int y = y0; y <= y0 + H; y++)
    {
        if (y == y0 + H || y == y0)
        {
            for (int x = x0; x <= x0 + a; x++)
            {
                TFTDisplay_ILI9341_DrawPixel(x, y, TFT_COLOR_ILI9341_BLACK);
            }
        }
        for (int x = x0; x <= x0 + a; x++)
        {
            if ((x == x0) || (x == x0 + a))
                TFTDisplay_ILI9341_DrawPixel(x, y, TFT_COLOR_ILI9341_BLACK);
        }
    }

    float h;
    if (dh_WT > 0 && dh_WT <= H_WT / 100)
        h = 0.1 * H;
    if (dh_WT > H_WT / 100 && dh_WT <= H_WT / 90)
        h = 0.1 * H;
    if (dh_WT > H_WT / 90 && dh_WT <= H_WT / 80)
        h = 0.2 * H;
    if (dh_WT > H_WT / 80 && dh_WT <= H_WT / 70)
        h = 0.3 * H;
    if (dh_WT > H_WT / 70 && dh_WT <= H_WT / 60)
        h = 0.4 * H;
    if (dh_WT > H_WT / 60 && dh_WT <= H_WT / 50)
        h = 0.5 * H;
    if (dh_WT > H_WT / 50 && dh_WT <= H_WT / 40)
        h = 0.6 * H;
    if (dh_WT > H_WT / 40 && dh_WT <= H_WT / 30)
        h = 0.7 * H;
    if (dh_WT > H_WT / 30 && dh_WT <= H_WT / 20)
        h = 0.8 * H;
    if (dh_WT > H_WT / 20 && dh_WT <= H_WT / 10)
        h = 0.9 * H;
    if (dh_WT > H_WT / 10 && dh_WT <= H_WT)
        h = H;

    for (int y = y0; y <= y0 + H; y++)
    {
        if (y <= y0 + H && y >= y0 + H - h)
        {
            for (int x = x0; x <= x0 + a; x++)
            {
                TFTDisplay_ILI9341_DrawPixel(x, y, TFT_COLOR_ILI9341_BLUE);
            }
        }
    }

    return h;
}

void Pipe_Out(int a, int H, int x0, int y0, float h, float h_WT, float H_MAX)
{
    for (int y = y0; y <= y0 + H; y++)
    {
        if (y == y0 + H || y == y0)
        {
            for (int x = x0; x <= x0 + a; x++)
            {
                TFTDisplay_ILI9341_DrawPixel(x, y, TFT_COLOR_ILI9341_BLACK);
            }
        }
        for (int x = x0; x <= x0 + a; x++)
        {
            if ((x == x0) || (x == x0 + a))
                TFTDisplay_ILI9341_DrawPixel(x, y, TFT_COLOR_ILI9341_BLACK);
        }
    }
    float hSaturated;
    if (h_WT < h && h != 0)
    {
        hSaturated = h_WT;
    }
    else if (h != 0)
    {
        hSaturated = h;
    }

    for (int y = y0; y <= y0 + H; y++)
    {
        if (y <= y0 + H && y >= y0 + H - hSaturated)
        {
            for (int x = x0; x <= x0 + a; x++)
            {
                TFTDisplay_ILI9341_DrawPixel(x, y, TFT_COLOR_ILI9341_BLUE);
            }
        }
    }
}