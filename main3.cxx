#include <iostream>
#include "SolveSchoredinger.hxx"
#include <iomanip>

int main()
{
    [[maybe_unused]] Wavefunction::Wavefunction<double,100,0.,10.,100,0.,10.> wavefunction;

    for (std::size_t r = 0; r < 100; ++r)
    {
        std::cout << wavefunction.At(r,3) << "\n";
    }
}