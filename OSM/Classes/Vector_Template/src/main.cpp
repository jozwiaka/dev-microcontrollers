#include <iostream>
#include "vector.h"

int main()
{
    Vector<int> v;
    v.push_back(1);
    v.clear();
    v.push_back(2);
    v.push_back(3);
    v.erase(v.begin() + 1);
    for (auto &vx : v)
    {
        std::cout << vx << ", ";
    }
    // std::cout << v;
}