#include "UAR.h"
#include "InercModel.h"

InercModel::InercModel(float pole) : UAR::UAR(0, 0)
{
    m_pole = pole;
}
void InercModel::Calculate()
{
    m_out = m_out * m_pole + (1 - m_pole) * m_in;
}