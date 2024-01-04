#include "UAR.h"
#include "PID.h"

PID::PID(float kp, float Ti, float Td, float Tp, float Smin, float Smax) : UAR::UAR()
{
    this->Tp = Tp;
    this->kp = kp;
    ki = kp * Tp / Ti;
    kd = kp * Td / Tp;
    this->Smin = Smin;
    this->Smax = Smax;
    inPrev=0;
    sum=0;
}
void PID::Calculate()
{
    sum += in * ki;
    if (sum > Smax)
        sum = Smax;
    if (sum < Smin)
        sum = Smin;
    out = sum + in * kp + (in-inPrev) * kd;
    inPrev=in;
}
