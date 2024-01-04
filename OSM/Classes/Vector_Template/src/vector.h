#ifndef VECTORH
#define VECTORH
#include <iostream>

template <typename T>
class Vector
{
private:
    T *data;
    int n;

public:
    Vector();
    Vector(int n);
    Vector(int n, T *data);
    ~Vector();                     // 1. Destructor
    Vector(const Vector &v);       // 2. Copy Constructor
    Vector &operator=(Vector &v);  // 3. Copy Assignment Operator
    Vector(Vector &&v);            // 4. Move Constructor
    Vector &operator=(Vector &&v); // 5. Move Assignment Operator
    int size() const;
    T operator[](int i) const;
    // std::ostream &operator<<(std::ostream &os, Vector &v) friend;
    T *begin();
    T *end();
    const T *begin() const;
    const T *end() const;
    void push_back(T d);
    void erase(T *it);
    void clear();
};
#endif