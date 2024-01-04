#ifndef PIDH
#define PIDH
#include "UAR.h"
class PID : public UAR
{
private:
    float Tp, kp, ki, kd;
    float Smin, Smax;
    float sum, inPrev;

public:
    PID(float kp, float Ti, float Td,
        float Tp, float Smin, float Smax);
    void Calculate();
};
#endif
