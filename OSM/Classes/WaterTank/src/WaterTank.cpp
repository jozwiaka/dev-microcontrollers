#include "WaterTank.h"

WaterTank::WaterTank(int a, int H, int x0, int y0, float yWy, float yWyPrev, float HReal) : a(a), H(H), x0(x0), y0(y0), yWy(yWy), yWyPrev(yWyPrev), HReal(HReal)
{
    h = yWy * H / HReal;
    dh = (float)(yWy - yWyPrev) * H / HReal;
    a_P = a / 4;
    a_T = a - 2 * a_P;
    H_P = H / 10;
    vMax = (float)H / (a * a);
    h_P1 = 0;
    h_P2 = 0;

    x0_T = x0 + a_P;
    y0_T = y0;
    x0_P1 = x0;
    y0_P1 = y0;
    x0_P2 = x0 + a_P + a_T;
    y0_P2 = y0 + H - H_P;
    x0_WA = x0 + a_P;
    y0_WA = y0 + H_P;
}
void WaterTank::Show()
{
}
void WaterTank::Tank()
{
}
void WaterTank::Pipe1()
{
}
void WaterTank::Pipe2()
{
}
void WaterTank::WaterAnimation()
{
    if (dh > 0 && dh <= H / 100)
    {
        v = 50 * vMax;
    }
    if (dh > H / 100 && dh <= H / 90)
    {
        v = 45 * vMax;
    }
    if (dh > H / 90 && dh <= H / 80)
    {
        v = 40 * vMax;
    }
    if (dh > H / 80 && dh <= H / 70)
    {
        v = 35 * vMax;
    }
    if (dh > H / 70 && dh <= H / 60)
    {
        v = 30 * vMax;
    }
    if (dh > H / 60 && dh <= H / 50)
    {
        v = 25 * vMax;
    }
    if (dh > H / 50 && dh <= H / 40)
    {
        v = 20 * vMax;
    }
    if (dh > H / 40 && dh <= H / 30)
    {
        v = 15 * vMax;
    }
    if (dh > H / 30 && dh <= H / 20)
    {
        v = 10 * vMax;
    }
    if (dh > H / 20 && dh <= H / 10)
    {
        v = 5 * vMax;
    }
    if (dh > H / 10 && dh <= H)
    {
        v = 1 * vMax;
    }
    int y = 0;
    if (dh > 0)
    {
        for (int x = x0; x <= x0 + a; x++)
        {
            for (int i = 0; i <= h_P1; i++)
            {
                y = y0 + v * (x - x0) * (x - x0) - i;
                if (y < y0 + H && x < x0 + a)
                    SetPixel(ekran, x, y);
            }
        }
    }
}