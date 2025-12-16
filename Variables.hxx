#ifndef Variables_hxx
    #define Variables_hxx

    #include "Expressions.hxx"

    namespace Variables
    {
        struct R 
        {
            double r;
            constexpr double operator()() {return r;}
        };

        struct K
        {
            double k;
            constexpr double operator()() {return k;}
        };

        constexpr Variable<R> r_var;
        constexpr Variable<K> k_var;
    }

#endif