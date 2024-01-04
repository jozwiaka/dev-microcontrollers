#ifndef PIDH
#define PIDH
#include "UAR.h"
class PID : public UAR
{
    float m_Tp, m_Kp, m_Ki, m_Kd;
    float m_sum, m_Smin, m_Smax;
    float m_prevIn;

public:
    PID(float Kp, float Ti, float Td,
        float Tp, float Smin, float Smax);
    void Calculate() override;
};
#endif