#include <iostream>
#include <vector>
#include "Expressions.hxx"

struct X
{
    double x;
    constexpr double operator()() {return x;}
};

struct Y
{
    double y;
    constexpr double operator()() {return y;}
};

int main()
{
    Variable<X> x;
    Variable<Y> y;
    Function f = x + y; // x + y
    Function g = x * y; // xy

    Function h = f * g; // x^2y + xy^2
    Function d2h = D(D(h,DiffWrt(x)),DiffWrt(x)); // double differencial wrt. x, i.e. d^2h/dx^2 = 2y

    std::vector<double> vals{-3,-2,-1,0,1,2,3};
    for (const auto &valx : vals)
    {
        for (const auto &valy : vals)
        {
            std::cout << h(X{valx},Y{valy}) << "\t";
        }
        std::cout << "\n";
    }
    std::cout << "\n\n\n";

    for (const auto &valx : vals)
    {
        for (const auto &valy : vals)
        {
            std::cout << d2h(X{valx},Y{valy}) << "\t";
        }
        std::cout << "\n";
    }

}