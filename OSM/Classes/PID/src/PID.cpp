#include "UAR.h"
#include "PID.h"

PID::PID(float Kp, float Ti, float Td, float Tp, float Smin, float Smax) : UAR::UAR()
{
    m_Tp = Tp;
    m_Kp = Kp;
    m_Ki = Kp * Tp / Ti;
    m_Kd = Kp * Td / Tp;
    m_prevIn = 0;
    m_sum = 0;
    m_Smin = Smin;
    m_Smax = Smax;
}
void PID::Calculate()
{
    float deltaIn = m_in - m_prevIn;
    m_sum += m_in * m_Ki;
    if (m_sum > m_Smax)
        m_sum = m_Smax;
    if (m_sum < m_Smin)
        m_sum = m_Smin;
    m_out = m_sum + m_in * m_Kp + deltaIn * m_Kd;
}