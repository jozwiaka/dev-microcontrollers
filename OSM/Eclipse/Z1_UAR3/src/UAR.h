#ifndef UARH
#define UARH
class UAR
{
protected:
    float in;
    float out;

public:
    UAR() : in(0), out(0) {}

    void setInput(float in)
    {
        this->in = in;
    }
    virtual void Calculate()
    {
        out = in;
    }

    float getOutput()
    {
        return out;
    }
};
#endif
