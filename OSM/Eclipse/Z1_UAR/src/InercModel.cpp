#include "UAR.h"
#include "InercModel.h"

InercModel::InercModel(float pole) : UAR::UAR(), pole(pole)
{
}
void InercModel::Calculate()
{
    out = out * pole + (1 - pole) * in;
}
