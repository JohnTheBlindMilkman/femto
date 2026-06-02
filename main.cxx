#include <iostream>
#include <future>
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

using namespace femto;

int main()
{
    constexpr Variable<X> x;
    // Variable<Y> y;
    constexpr Constant a(2.);
    constexpr Function f = pow(x,a);
    constexpr double result = f(X{2});

    std::cout << result << "\n";
}