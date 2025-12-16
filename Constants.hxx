#ifndef Constants_hxx
    #define COnstants_hxx

    #include "Expressions.hxx"
    #include <numeric>

    namespace Constants
    {
        inline constexpr Constant protonMassMeV(938.27208816);
        inline constexpr Constant fineStructure(0.007297);
        inline constexpr Constant piConst(std::numbers::pi);
        inline constexpr Function elementaryCharge = Sqrt(Constant(4) * piConst * fineStructure);
    }

#endif