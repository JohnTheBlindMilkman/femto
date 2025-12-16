#ifndef Wavefunction_hxx
    #define Wavefunction_hxx

    #include "ShortRangePotentials.hxx"
    #include "Array2D.hxx"
    #include "Constants.hxx"

    namespace Wavefunction
    {
        template <typename T, std::size_t SizeR, T MinR, T MaxR, std::size_t SizeK, T MinK, T MaxK>
        class Wavefunction
        {
            private:
                const Utils::Array2D<T,SizeR,SizeK> m_wavefunction;
                static constexpr Function m_coulombStrongFunc = Variables::k_var * Variables::k_var - (Constant(4) * Constants::elementaryCharge * Constants::elementaryCharge * Constants::protonMassMeV) / Variables::r_var - Constant(4) * Constants::protonMassMeV * Potential::ReidSoftCore::pot_1S0;
                static constexpr Function m_coulombFunc = Variables::k_var * Variables::k_var - (Constant(4) * Constants::elementaryCharge * Constants::elementaryCharge * Constants::protonMassMeV) / Variables::r_var;
                static constexpr double m_rStep = static_cast<double>(MaxR - MinR) / SizeR;
                static constexpr double m_kStep = static_cast<double>(MaxK - MinK) / SizeK;

                constexpr T CalculateRValue(std::size_t rIter) const noexcept
                {
                    return MinR + m_rStep * rIter;
                }
                constexpr T CalculateKValue(std::size_t kIter) const noexcept
                {
                    return MinK + m_kStep * kIter;
                }
                [[nodiscard]] constexpr T Numerov(const Utils::Array2D<T,SizeR,SizeK> &arr, const auto &func, double rStep, std::size_t rIter, std::size_t kIter) const noexcept
                {
                    if (rIter == 0 || kIter == 0)
                    {
                        return T(0);
                    }
                    else if (rIter == 1 || kIter == 1)
                    {
                        return Math::Sqrt(std::numeric_limits<T>::epsilon());
                    }
                    else
                    {
                        return (2. * arr.at(rIter,kIter) * (1. - 5. / 12. * rStep * rStep * func(Variables::R{CalculateRValue(rIter)},Variables::K{CalculateKValue(kIter)})) -
                            arr.at(rIter - 1, kIter - 1) * (1. + rStep * rStep / 12. * func(Variables::R{CalculateRValue(rIter - 1)},Variables::K{CalculateKValue(kIter - 1)}))) /
                            (1. + rStep * rStep / 12. * func(Variables::R{CalculateRValue(rIter + 1)},Variables::K{CalculateKValue(kIter + 1)})); 
                    }
                }
                constexpr Utils::Array2D<T,SizeR,SizeK> CalculateWavefunction() const noexcept
                {
                    Utils::Array2D<T,SizeR,SizeK> tmp_array{};
                    for (std::size_t rIter = 0; rIter < SizeR - 1; ++rIter)
                    {
                        for (std::size_t kIter = 0; kIter < SizeK - 1; ++kIter)
                        {
                            tmp_array.at(rIter + 1,kIter + 1) = Numerov(tmp_array, m_coulombStrongFunc, m_rStep, rIter, kIter);
                        }
                    }

                    return tmp_array;
                }

            public:
                constexpr Wavefunction() requires (MaxR > MinR && MaxK > MinK) : 
                    m_wavefunction(CalculateWavefunction()) {}
                [[nodiscard]] constexpr T Evaluate(T pos, T mom) noexcept
                {
                    return T(0); // to be implemented later (tip: use std::lerp)
                }
                [[nodiscard]] constexpr T At(std::size_t r, std::size_t k) const noexcept
                {
                    return m_wavefunction.at(r,k);
                }
        };
    }

#endif