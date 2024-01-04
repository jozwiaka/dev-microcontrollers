#include "vector.h"

template class Vector<int>; // forward declaration

template <typename T>
Vector<T>::Vector() : n(0), data(nullptr)
{
}

template <typename T>
Vector<T>::Vector(int n) : n(n), data(new T[n])
{
    for (int i = 0; i < n; i++)
    {
        data[i] = 0;
    }
}

template <typename T>
Vector<T>::Vector(int n, T *data) : n(n), data(data)
{
}

template <typename T>
Vector<T>::~Vector() // 1. Destructor
{
    delete[] data;
}

template <typename T>
Vector<T>::Vector(const Vector<T> &v) // 2. Copy Constructor
{

    n = v.n;
    data = new T[n];
    for (int i = 0; i < n; i++)
    {
        data[i] = v.data[i];
    }
}

template <typename T>
Vector<T> &Vector<T>::operator=(Vector<T> &v) // 3. Copy Assignment Operator
{
    if (this == &v)
    {
        return *this;
    }
    delete[] data;
    n = v.n;
    data = new T[n];
    for (int i = 0; i < n; i++)
    {
        data[i] = v.data[i];
    }
    return *this;
}

template <typename T>
Vector<T>::Vector(Vector<T> &&v) // 4. Move Constructor
{
    data = v.data;
    n = v.n;
    v.data = nullptr;
    v.n = 0;
}

template <typename T>
Vector<T> &Vector<T>::operator=(Vector &&v) // 5. Move Assignment Operator
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

template <typename T>
int Vector<T>::size() const
{
    return n;
}

// T &operator[](int i) const
// {
//     return data[i];
// }

template <typename T>
T Vector<T>::operator[](int i) const
{
    return data[i];
}

// template <typename T>
// std::ostream &operator<<(std::ostream &os, Vector<T> &v)
// {
//     for (auto &d : v)
//     {
//         os << d << ", ";
//     }
//     return os;
// }

template <typename T>
T *Vector<T>::begin()
{
    return data;
}

template <typename T>
T *Vector<T>::end()
{
    return data + n;
}

template <typename T>
const T *Vector<T>::begin() const
{
    return data;
}

template <typename T>
const T *Vector<T>::end() const
{
    return data + n;
}

template <typename T>
void Vector<T>::push_back(T d)
{
    T *temp = data;
    n = n + 1;
    data = new T[n];
    for (int i = 0; i < n; i++)
    {
        if (i == n - 1)
            data[i] = d;
        else
            data[i] = temp[i];
    }
    delete[] temp;
}

template <typename T>
void Vector<T>::erase(T *it)
{
    T *temp = data;
    n = n - 1;
    data = new T[n];
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

template <typename T>
void Vector<T>::clear()
{
    delete[] data;
    data = nullptr;
    n = 0;
}