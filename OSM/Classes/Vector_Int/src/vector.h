#ifndef VECintORH
#define VECintORH
#include <iostream>

class Vector
{
private:
    int *data;
    int n;

public:
    Vector();
    Vector(int n);
    Vector(int n, int *data);
    ~Vector();                     // 1. Destructor
    Vector(const Vector &v);       // 2. Copy Constructor
    Vector &operator=(Vector &v);  // 3. Copy Assignment Operator
    Vector(Vector &&v);            // 4. Move Constructor
    Vector &operator=(Vector &&v); // 5. Move Assignment Operator
    int size() const;
    int &operator[](int i) const;
    // std::ostream &operator<<(std::ostream &os, Vector &v) friend;
    int *begin();
    int *end();
    const int *begin() const;
    const int *end() const;
    void push_back(int d);
    void erase(int *it);
    void clear();
};
#endif