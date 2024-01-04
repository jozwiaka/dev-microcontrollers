#include <iostream>
#include "vector.h"

int main()
{
    Vector v1;
    v1.push_back(1);
    v1.clear();
    v1.push_back(2);
    v1.push_back(3);
    v1.erase(v1.begin() + 1);
    for (auto &vx : v1)
    {
        std::cout << vx << ", ";
    }
    std::cout << std::endl;
    int *i = new int[5]{1, 2, 3, 4, 5};
    Vector v2 = Vector(5, i);
    for (auto &vx : v2)
    {
        std::cout << vx << ", ";
    }
    std::cout << std::endl;
    Vector v3 = Vector(4);
    for (auto &vx : v3)
    {
        vx = 1;
        std::cout << vx << ", ";
    }
    for (int i = 0; i < v3.size(); i++)
    {
        v3[i] = i;
        std::cout << v3[i] << ", ";
    }
}