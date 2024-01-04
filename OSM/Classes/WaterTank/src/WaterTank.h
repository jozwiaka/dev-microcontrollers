#ifndef WATERTANKH
#define WATERTANKH
class WaterTank
{
private:
    int a;
    int H;
    int x0;
    int y0;
    float yWy;
    float yWyPrev;
    float HReal;

    float h;
    float h_P1;
    float h_P2;
    int x0_T;
    int y0_T;
    int x0_P1;
    int y0_P1;
    int x0_P2;
    int y0_P2;
    int x0_WA;
    int y0_WA;
    float dh;
    int a_P;
    int a_T;
    int H_P;
    int H_P_W;
    float v;
    float vMax;

private:
    void Tank();
    void WaterAnimation();
    void Pipe1();
    void Pipe2();

public:
    WaterTank(int, int, int, int, float, float, float);
    void Show();
};
#endif