#ifndef ShortRangePotentials_hxx
    #define ShortRangePotentials_hxx

    #include "Expressions.hxx"
    #include "Variables.hxx"

    namespace Potential
    {
        namespace ReidSoftCore
        {
            namespace Detail
            {
                constexpr Constant h_const(10.463);
                constexpr Constant mu_const(0.7);
                constexpr Function mu_r = Variables::r_var * mu_const;
            }

            inline constexpr Function pot_1S0 = -(Detail::h_const * Exp(-Detail::mu_r)) / Detail::mu_r - (Constant(1650.6) * Exp(-Constant(4) * Detail::mu_r)) / Detail::mu_r - (Constant(6484.2) * Exp(-Constant(7) * Detail::mu_r)) / Detail::mu_r;
            // inline constexpr Function pot_3P0;
            // inline constexpr Function pot_3P1;
            // inline constexpr Function pot_3C2;
        }
    }

#endif