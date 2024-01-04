#ifndef UARH
#define UARH
class UAR
{
protected:
    float m_out;
    float m_in;

public:
    UAR(float m_out, float m_in) : m_out(m_out), m_in(m_in) {} // added

    void setInput(float input)
    {
        m_in = input;
    }
    virtual void Calculate()
    {
        m_out = m_in;
    }
    float getOutput()
    {
        return m_out;
    }
};
#endif