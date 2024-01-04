#ifndef InercModelH
#define InercModelH

#include "UAR.h"
class InercModel : public UAR
{
    float pole;

public:
    InercModel(float pole);
    void Calculate();
};

#endif
