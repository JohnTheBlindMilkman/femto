#include <iostream>
#include <vector>
#include "Expressions.hxx"

int main()
{
    Function f([](double x, double y){return x  + y;});
    Function g([](double x, double y){return x * y;});

    Function h = f * g;
    //Function dh = Derivative(Derivative(h));

    std::vector<int> vals{-3,-2,-1,0,1,2,3};
    for (const auto &valx : vals)
    {
        for (const auto &valy : vals)
        {
            std::cout << h(valx,valy) << "\t";
        }
        std::cout << "\n";
    }
}