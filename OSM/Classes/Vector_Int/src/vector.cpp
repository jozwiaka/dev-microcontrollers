#include "vector.h"

Vector::Vector() : n(0), data(nullptr)
{
}

Vector::Vector(int n) : n(n), data(new int[n])
{
    for (int i = 0; i < n; i++)
    {
        data[i] = 0;
    }
}

Vector::Vector(int n, int *data) : n(n), data(data)
{
}

Vector::~Vector() // 1. Destructor
{
    delete[] data;
}

Vector::Vector(const Vector &v) // 2. Copy Constructor
{

    n = v.n;
    data = new int[n];
    for (int i = 0; i < n; i++)
    {
        data[i] = v.data[i];
    }
}

Vector &Vector::operator=(Vector &v) // 3. Copy Assignment Operator
{
    if (this == &v)
    {
        return *this;
    }
    delete[] data;
    n = v.n;
    data = new int[n];
    for (int i = 0; i < n; i++)
    {
        data[i] = v.data[i];
    }
    return *this;
}

Vector::Vector(Vector &&v) // 4. Move Constructor
{
    data = v.data;
    n = v.n;
    v.data = nullptr;
    v.n = 0;
}

Vector &Vector::operator=(Vector &&v) // 5. Move Assignment Operator
{
    if (this == &v)
    {
        return *this;
    }
    delete[] data;
    data = v.data;
    n = v.n;
    v.data = nullptr;
    v.n = 0;
    return *this;
}

int Vector::size() const
{
    return n;
}

int &Vector::operator[](int i) const
{
    return data[i];
}

//
// std::ostream &operator<<(std::ostream &os, Vector &v)
// {
//     for (auto &d : v)
//     {
//         os << d << ", ";
//     }
//     return os;
// }

int *Vector::begin()
{
    return data;
}

int *Vector::end()
{
    return data + n;
}

const int *Vector::begin() const
{
    return data;
}

const int *Vector::end() const
{
    return data + n;
}

void Vector::push_back(int d)
{
    int *temp = data;
    n = n + 1;
    data = new int[n];
    for (int i = 0; i < n; i++)
    {
        if (i == n - 1)
            data[i] = d;
        else
            data[i] = temp[i];
    }
    delete[] temp;
}

void Vector::erase(int *it)
{
    int *temp = data;
    n = n - 1;
    data = new int[n];
    int j = 0;
    for (int i = 0; i < n + 1; i++)
    {
        if (&temp[i] == it)
        {
            continue;
        }
        data[j] = temp[i];
        j++;
    }
    delete[] temp;
}
void Vector::clear()
{
    delete[] data;
    data = nullptr;
    n = 0;
}