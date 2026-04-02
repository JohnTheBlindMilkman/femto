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
    femto::Variable<X> x;
    femto::Variable<Y> y;
    femto::Function f = x + y; // x + y
    femto::Function g = x * y; // xy

    femto::Function h = f * g; // x^2y + xy^2
    femto::Function d2h = femto::d(femto::d(h,femto::diff_wrt(x)),femto::diff_wrt(x)); // double differencial wrt. x, i.e. d^2h/dx^2 = 2y

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