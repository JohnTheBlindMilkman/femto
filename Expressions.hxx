#ifndef Expressions_hxx
    #define Expressions_hxx

    #include <array>
    #include <cmath>

    #include "ConstexprMath.hxx"

    namespace femto
    {
        namespace detail
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
                    return lhsFunc.template Diff<W>() + rhsFunc.template Diff<W>();
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
            
            struct Power : public Expression<Power>
            {
                template <typename T, typename U, typename W>
                [[nodiscard]] constexpr decltype(auto) Diff(T base, U exponent) const
                {
                    // (a ^ b)' = (exp(b * ln(a)))' = a^b * (b' * ln(a) + b / a * a')
                    return pow(base,exponent) * (exponent.template Diff<W>() * ln(base) + exponent / base * base.template Diff<W>());
                }
                constexpr decltype(auto) operator()(const Arithmetic auto &lhs, const Arithmetic auto &rhs) const
                {
                    return math::pow(lhs,rhs);
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

        template <typename T, typename U>
        constexpr BinaryOp<T,U,BinaryOperations::Power> pow(T t, U u)
        {
            return BinaryOp<T,U,BinaryOperations::Power>(std::forward<T>(t),std::forward<U>(u));
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
                // exp(x)' = exp(x) * x'
                template <typename T, typename U>
                [[nodiscard]] constexpr decltype(auto) Diff(T func) const
                {
                    return exp(func) * func.template Diff<U>();
                }
                constexpr decltype(auto) operator()(const Arithmetic auto &val) const
                {
                    return math::exp(val);
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
                // (sqrt(x))' = (x^(1/2))' = 1/2 * x^(-1/2) * x'
                template <typename T, typename U>
                [[nodiscard]] constexpr decltype(auto) Diff(T func) const
                {
                    return Constant(0.5) / sqrt(func) * func.template Diff<U>();
                }
                constexpr decltype(auto) operator()(const Arithmetic auto &val) const
                {
                    return math::sqrt(val);
                }
            }; 

            struct NaturalLog : public Expression<NaturalLog>
            {
                // (lnx)' = 1/x * x'
                template <typename T, typename U>
                [[nodiscard]] constexpr decltype(auto) Diff(T func) const
                {
                    return Constant(1.) / func * func.template Diff<U>();
                }
                constexpr decltype(auto) operator()(const Arithmetic auto &val) const
                {
                    return math::ln(val);
                }
            };
            
        } // namespace UnaryOperations
        
        template <typename T>
        constexpr UnaryOp<T,UnaryOperations::Exponential> exp(T t)
        {
            return UnaryOp<T,UnaryOperations::Exponential>(std::forward<T>(t));
        }

        template <typename T>
        constexpr UnaryOp<T,UnaryOperations::Negation> operator-(T t)
        {
            return UnaryOp<T,UnaryOperations::Negation>(std::forward<T>(t));
        }

        template <typename T>
        constexpr UnaryOp<T,UnaryOperations::SquareRoot> sqrt(T t)
        {
            return UnaryOp<T,UnaryOperations::SquareRoot>(std::forward<T>(t));
        }

        template <typename T>
        constexpr UnaryOp<T,UnaryOperations::NaturalLog> ln(T t)
        {
            return UnaryOp<T,UnaryOperations::NaturalLog>(std::forward<T>(t));
        }

        template <typename T, typename U>
        class Derivative : public Expression<Derivative<T,U> >
        {
            private:
                const T m_t;
                const detail::DiffVariable<U> m_diff;

            public:
                constexpr Derivative(T t, detail::DiffVariable<U> diff) : m_t(std::forward<T>(t)), m_diff(std::forward<detail::DiffVariable<U> >(diff)) {}
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
        constexpr detail::DiffVariable<T> diff_wrt(Variable<T>) {return detail::DiffVariable<T>{};}

        template <typename T, typename U>
        constexpr Derivative<T,U> d(T t, detail::DiffVariable<U> &&u)
        {
            return Derivative<T,U>(std::forward<T>(t),std::forward<detail::DiffVariable<U> >(u));
        }
    }

#endif