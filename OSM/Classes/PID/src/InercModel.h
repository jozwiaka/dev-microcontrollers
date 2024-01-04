#ifndef InercModelH
#define InercModelH

#include "UAR.h"
class InercModel : public UAR
{
    float m_pole;

public:
    InercModel(float pole);
    void Calculate() override;
};

#endif