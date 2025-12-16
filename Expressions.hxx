#ifndef Expressions_hxx
    #define Expressions_hxx

    #include <array>
    #include <cmath>

    #include "ConstexprMath.hxx"

    namespace Detail
    {
        template <typename T>
        struct DiffVariable
        {
            using type = T;
        };
    }

    template <typename T>
    class Expression
    {
        public:         
            template <typename ...Args>
            [[nodiscard]] constexpr decltype(auto) operator()(Args &&... args) const requires Functionlike<T,Args...> 
            {
                return static_cast<const T&>(*this)(std::forward<Args>(args)...);
            }
    };

    template <typename T>
    class Function : public Expression<Function<T> >
    {
        private:
            const T m_callable;

        public:
            constexpr Function(T func) : m_callable(std::forward<T>(func)) {}
            template <typename U>
            [[nodiscard]] constexpr decltype(auto) Diff() const
            {
                return m_callable.template Diff<U>();
            }
            template <typename ...Args>
            [[nodiscard]] constexpr decltype(auto) operator()(Args &&... args) const requires Functionlike<T,Args...>
            {
                return m_callable(std::forward<Args>(args)...);
            }
    };

    template <Arithmetic T>
    class Constant : public Expression<Constant<T> >
    {
        private:
            const T m_value;

        public:
            constexpr Constant(T val) : m_value(std::forward<T>(val)) {}
            template <typename U>
            [[nodiscard]] constexpr decltype(auto) Diff() const
            {
                return Constant(0);
            }
            template <typename ...Args>
            [[nodiscard]] constexpr T operator()(Args &&...) const {return m_value;}
    };

    template <typename T>
    class Variable : public Expression<Variable<T> >
    {
        public:
            template <typename U>
            [[nodiscard]] constexpr decltype(auto) Diff() const
            {
                if constexpr (std::is_same_v<T,U>)
                {
                    return Constant(1);
                }
                else
                {
                    return Constant(0);
                }
            }
            template <typename Var, typename ... Vars>
            [[nodiscard]] constexpr decltype(auto) operator()(Var &&first, Vars &&... rest) const
            {
                if constexpr (std::is_same_v<T,Var>)
                {
                    return first();
                }
                else
                {
                    return operator()(std::forward<Vars>(rest)...);
                }
            }
    };

    template <typename T, typename U, typename Op>
    class BinaryOp : public Expression<BinaryOp<T,U,Op> >
    {
        private:
            const T m_t;
            const U m_u;

        public:
            constexpr BinaryOp(T t, U u) : m_t(std::forward<T>(t)), m_u(std::forward<U>(u)) {}
            template <typename W>
            [[nodiscard]] constexpr decltype(auto) Diff() const
            {
                return Op{}.template Diff<T,U,W>(m_t,m_u);
            }
            template <typename ...Args>
            [[nodiscard]] constexpr decltype(auto) operator()(Args &&... args) const requires Functionlike<T,Args...>
            {
                return Op{}(m_t(std::forward<Args>(args)...),m_u(std::forward<Args>(args)...));
            }
    };

    namespace BinaryOperations
    {
        struct Sum : public Expression<Sum>
        {
            template <typename T, typename U, typename W>
            [[nodiscard]] constexpr decltype(auto) Diff(T lhsFunc, U rhsFunc) const
            {
                // (a + b)' = a' + b'
                return lhsFunc.template Diff<W>() + 
                    rhsFunc.template Diff<W>();
            }
            constexpr decltype(auto) operator()(const Arithmetic auto &lhs, const Arithmetic auto &rhs) const
            {
                return lhs + rhs;
            }
        };

        struct Difference : public Expression<Difference>
        {
            template <typename T, typename U, typename W>
            [[nodiscard]] constexpr decltype(auto) Diff(T lhsFunc, U rhsFunc) const
            {
                // (a - b)' = a' - b'
                return lhsFunc.template Diff<W>() - rhsFunc.template Diff<W>();
            }
            constexpr decltype(auto) operator()(const Arithmetic auto &lhs, const Arithmetic auto &rhs) const
            {
                return lhs - rhs;
            }
        };

        struct Multiplication : public Expression<Multiplication>
        {
            template <typename T, typename U, typename W>
            [[nodiscard]] constexpr decltype(auto) Diff(T lhsFunc, U rhsFunc) const
            {
                // (a * b)' = a' * b + a * b'
                return lhsFunc.template Diff<W>() * rhsFunc + lhsFunc * rhsFunc.template Diff<W>();
            }
            constexpr decltype(auto) operator()(const Arithmetic auto &lhs, const Arithmetic auto &rhs) const
            {
                return lhs * rhs;
            }
        };

        struct Division : public Expression<Division>
        {
            template <typename T, typename U, typename W>
            [[nodiscard]] constexpr decltype(auto) Diff(T lhsFunc, U rhsFunc) const
            {
                // (a / b)' = (a' * b - a * b') / (b * b)
                return (lhsFunc.template Diff<W>() * rhsFunc - lhsFunc * rhsFunc.template Diff<W>()) / (rhsFunc * rhsFunc);
            }
            constexpr decltype(auto) operator()(const Arithmetic auto &lhs, const Arithmetic auto &rhs) const
            {
                return lhs / rhs;
            }
        };
           
    } // namespace BinaryOperations

    template <typename T, typename U>
    constexpr BinaryOp<T,U,BinaryOperations::Sum> operator+(T t, U u)
    {
        return BinaryOp<T,U,BinaryOperations::Sum>(std::forward<T>(t),std::forward<U>(u));
    }

    template <typename T, typename U>
    constexpr BinaryOp<T,U,BinaryOperations::Difference> operator-(T t, U u)
    {
        return BinaryOp<T,U,BinaryOperations::Difference>(std::forward<T>(t),std::forward<U>(u));
    }

    template <typename T, typename U>
    constexpr BinaryOp<T,U,BinaryOperations::Multiplication> operator*(T t, U u)
    {
        return BinaryOp<T,U,BinaryOperations::Multiplication>(std::forward<T>(t),std::forward<U>(u));
    }

    template <typename T, typename U>
    constexpr BinaryOp<T,U,BinaryOperations::Division> operator/(T t, U u)
    {
        return BinaryOp<T,U,BinaryOperations::Division>(std::forward<T>(t),std::forward<U>(u));
    }

    template <typename T, typename Op>
    class UnaryOp : public Expression<UnaryOp<T,Op> >
    {
        private:
            const T m_t;

        public:
            constexpr UnaryOp(T t) : m_t(std::forward<T>(t)) {}
            template <typename U>
            [[nodiscard]] constexpr decltype(auto) Diff() const
            {
                return Op{}.template Diff<T,U>(m_t);
            }
            template <typename ...Args>
            [[nodiscard]] constexpr decltype(auto) operator()(Args &&... args) const requires Functionlike<T,Args...>
            {
                return Op{}(m_t(std::forward<Args>(args)...));
            }
    };

    namespace UnaryOperations
    {
        struct Exponential : public Expression<Exponential>
        {
            // Exp(x)' = Exp(x) * x'
            template <typename T, typename U>
            [[nodiscard]] constexpr decltype(auto) Diff(T func) const
            {
                return Exp(func) * func.template Diff<U>();
            }
            constexpr decltype(auto) operator()(const Arithmetic auto &val) const
            {
                return Math::Exp(val);
            }
        };

        struct Negation : public Expression<Negation>
        {
            // (-x)' = -x'
            template <typename T, typename U>
            [[nodiscard]] constexpr decltype(auto) Diff(T func) const
            {
                return -func.template Diff<U>();
            }
            constexpr decltype(auto) operator()(const Arithmetic auto &val) const
            {
                return -val;
            }
        };

        struct SquareRoot : public Expression<SquareRoot>
        {
            // (Sqrt(x))' = (x^(1/2))' = 1/2 * x^(-1/2) * x'
            template <typename T, typename U>
            [[nodiscard]] constexpr decltype(auto) Diff(T func) const
            {
                return Constant(0.5) / Sqrt(func) * func.template Diff<U>();
            }
            constexpr decltype(auto) operator()(const Arithmetic auto &val) const
            {
                return Math::Sqrt(val);
            }
        };
        
    } // namespace UnaryOperations
    
    template <typename T>
    constexpr UnaryOp<T,UnaryOperations::Exponential> Exp(T t)
    {
        return UnaryOp<T,UnaryOperations::Exponential>(std::forward<T>(t));
    }

    template <typename T>
    constexpr UnaryOp<T,UnaryOperations::Negation> operator-(T t)
    {
        return UnaryOp<T,UnaryOperations::Negation>(std::forward<T>(t));
    }

    template <typename T>
    constexpr UnaryOp<T,UnaryOperations::SquareRoot> Sqrt(T t)
    {
        return UnaryOp<T,UnaryOperations::SquareRoot>(std::forward<T>(t));
    }

    template <typename T, typename U>
    class Derivative : public Expression<Derivative<T,U> >
    {
        private:
            const T m_t;
            const Detail::DiffVariable<U> m_diff;

        public:
            constexpr Derivative(T t, Detail::DiffVariable<U> diff) : m_t(std::forward<T>(t)), m_diff(std::forward<Detail::DiffVariable<U> >(diff)) {}
            template <typename W>
            [[nodiscard]] constexpr decltype(auto) Diff() const
            {
                return m_t.template Diff<U>(). template Diff<W>();
            }
            template <typename ... Args>
            [[nodiscard]] constexpr decltype(auto) operator()(Args &&... args) const requires Functionlike<T,Args...>
            {
                return m_t.template Diff<U>()(std::forward<Args>(args)...);
            }
    };

    template <typename T>
    constexpr Detail::DiffVariable<T> DiffWrt(Variable<T>) {return Detail::DiffVariable<T>{};}

    template <typename T, typename U>
    constexpr Derivative<T,U> D(T t, Detail::DiffVariable<U> &&u)
    {
        return Derivative<T,U>(std::forward<T>(t),std::forward<Detail::DiffVariable<U> >(u));
    }

#endif