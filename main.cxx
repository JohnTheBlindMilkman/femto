#include <iostream>
#include <random>
#include <iomanip>
#include "Expressions.hxx"

template <std::size_t N>
std::ostream& operator<<(std::ostream &os, std::array<double,N> arr)
{
    for (std::size_t i = 0; i < arr.size(); ++i)
    {
        os << arr[i] << " -> " << i << "\n";
    }

    return os;
}

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

struct Z
{
    double z;
    constexpr double operator()() {return z;}
};

struct R
{
    double x;
    constexpr double operator()() {return x;}
};

int main()
{
    for (const double &val : {-10.,-5.,-1.,0.,1.,5.,10.})
    {
        std::cout << std::setprecision(15) << "x = " << val << "\tf(x) = " << Math::Exp(val) << "\th(x) = " << std::exp(val) << "\n";
    }
}