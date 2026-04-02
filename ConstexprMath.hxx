#ifndef ConstexprMath_hxx
    #define ConstexprMath_hxx

    #include <limits>
    #include "Concepts.hxx"

	namespace femto
	{
		namespace math
		{
			namespace limits
			{
				template <typename T> struct Depth {static constexpr std::size_t limit = 0;};
				template <> struct Depth<short> {static constexpr std::size_t limit = 8;};
				template <> struct Depth<unsigned short> {static constexpr std::size_t limit = 11;};
				template <> struct Depth<int> {static constexpr std::size_t limit = 11;};
				template <> struct Depth<unsigned> {static constexpr std::size_t limit = 20;};
				template <> struct Depth<long> {static constexpr std::size_t limit = 20;};
				template <> struct Depth<unsigned long> {static constexpr std::size_t limit = 34;};
				template <> struct Depth<float> {static constexpr std::size_t limit = 34;};
				template <> struct Depth<double> {static constexpr std::size_t limit = 170;};
				template <> struct Depth<long double> {static constexpr std::size_t limit = 170;};
			}
			template <Arithmetic T>
			constexpr bool is_nan(T t) noexcept
			{
				return ! (t == t);
			}

			// factorial
			namespace detail_factorial
			{
				template<Arithmetic T, std::size_t ... Is>
				[[nodiscard]] consteval T factorial_impl(std::index_sequence<Is...>) noexcept
				{
					return ((Is + 1) * ... * 1); // * 1 in case the index sequence is empty and fold expression cannot fold anything
				}

				template<Arithmetic T, std::size_t N>
				[[nodiscard]] consteval T factorial() noexcept
				{
					return factorial_impl<T>(std::make_index_sequence<N>{});
				}

				template <Arithmetic T, std::size_t ... Is>
				[[nodiscard]] consteval std::array<T,limits::Depth<T>::limit + 1> make_factorials(std::index_sequence<Is...>) noexcept
				{
					return {{factorial<T,Is>()...}};
				}

				template <Arithmetic T> 
				static constexpr std::array<T,limits::Depth<T>::limit + 1> factorials = make_factorials<T>(std::make_index_sequence<limits::Depth<T>::limit>{});
			}

			template<Arithmetic T, std::size_t N>
			[[nodiscard]] constexpr T factorial() noexcept
			{
				return detail_factorial::factorials<T>[N];
			}

			// Helper function for creating sequence of same integer value
			template <std::size_t Value, std::size_t... Is>
			std::index_sequence<(Is, Value)...> make_sequence_helper(std::index_sequence<Is...>);  // no definition

			template<std::size_t Value, std::size_t Count>
			using make_index_sequence_of_same_value = decltype(make_sequence_helper<Value>(std::make_index_sequence<Count>()));

			// power with integral exponent
			namespace detail_power
			{
				template <Arithmetic T, std::size_t ... Is>
				[[nodiscard]] constexpr T power(T t, std::index_sequence<Is...>) noexcept
				{
					return ((Is * t) * ... * 1); // * 1 in case the index sequence is empty and fold expression cannot fold anything
				}
			}
			
			template <std::size_t N, Arithmetic T>
			[[nodiscard]] constexpr T power(T t) noexcept
			{
				return detail_power::power(t,make_index_sequence_of_same_value<1,N>());
			}

			// Exponential
			namespace detail_exp
			{
				template <std::size_t N, Arithmetic T>
				[[nodiscard]] constexpr T exp_impl(T t) noexcept
				{
					return power<N>(t) / factorial<T,N>();
				}

				template <Arithmetic T, std::size_t ... Is>
				[[nodiscard]] constexpr T exp(T t, std::index_sequence<Is...> ) noexcept
				{
					return (t > 0) ? T(1) + (exp_impl<Is + 1,T>(t) + ...) : T(1) / (T(1) + (exp_impl<Is + 1,T>(-t) + ...));
				}
			}

			template <Arithmetic T>
			[[nodiscard]] constexpr T exp(T t) noexcept
			{
				return (t == 0) ? T(1) : 
					(t == -std::numeric_limits<T>::infinity()) ? T(0) :
					(t == std::numeric_limits<T>::infinity()) ? std::numeric_limits<T>::infinity() :
					is_nan(t) ? std::numeric_limits<T>::quiet_NaN() :
					detail_exp::exp(t, std::make_index_sequence<limits::Depth<T>::limit>{});
			}

			// Square root
			namespace detail_sqrt
			{
				template <typename T>
				T constexpr sqrtNewtonRaphson(T x, T curr, T prev) noexcept
				{
					return curr == prev ? curr : sqrtNewtonRaphson<T>(x, 0.5 * (curr + x / curr), curr);
				}
			}

			template <Arithmetic T>
			[[nodiscard]] constexpr T sqrt(T t) noexcept
			{
				return t >= 0 && t < std::numeric_limits<T>::infinity() ? detail_sqrt::sqrtNewtonRaphson<T>(t, t, T(0)) : std::numeric_limits<T>::quiet_NaN();
			}

			//Natural logarithm
			namespace detail_ln
			{
				template <Arithmetic T, std::size_t ... Is>
				constexpr T ln_impl_2(T val, std::index_sequence<Is...>) noexcept // Taylor series expansion for ln(x+1) = \sum_{k = 1}^{inf} \frac{x^k}{k}
				{
					return ((((Is + 1) % 2 == 1 ? 1 : -1) * power<Is + 1>(val) / (Is + 1)) + ... + 0); // Is + 1, because sum is from 1 and index sequence starts from 0
				}

				template <Arithmetic T>
				constexpr T ln_impl_1(T val) noexcept
				{
					return (val <= sqrt(T(2))) ? ln_impl_2(val - T(1),std::make_index_sequence<limits::Depth<T>::limit>{}) : T(2) * ln_impl_1(sqrt(val)); // Taylor series is a valid approximation only around |x| < sqrt(2)
				}

				template <Arithmetic T>
				constexpr T ln_impl(T val) noexcept
				{
					return (val < 1) ? -ln_impl_1(T(1) / val) : ln_impl_1(val);
				}
			} // namespace detail_ln

			template <Arithmetic T>
			[[nodiscard]] constexpr T ln(T t) noexcept
			{
				return is_nan(t) ? t
					: t == 0 ? -std::numeric_limits<T>::infinity()
					: t == std::numeric_limits<T>::infinity() ? std::numeric_limits<T>::infinity()
					: t < 0 ? std::numeric_limits<T>::quiet_NaN()
					: t == 1 ? T(0)
					: detail_ln::ln_impl(t);
			}

			// power with real exponent
			template <Arithmetic T, Arithmetic U>
			[[nodiscard]] consteval T pow(T base, U exponent) noexcept
			{
				if constexpr (is_nan(base) || is_nan(exponent)) 
				{
					std::numeric_limits<T>::quiet_NaN();
				}
				else
				{
					if constexpr (base == -std::numeric_limits<T>::infinity())
					{
						return -std::numeric_limits<T>::infinity();
					}
					else if constexpr (base == std::numeric_limits<T>::infinity())
					{
						return std::numeric_limits<T>::infinity();
					}
					else
					{
						if constexpr (exponent == -std::numeric_limits<U>::infinity())
						{
							return T(0);
						}
						else if constexpr (exponent == T(0))
						{
							return T(1);
						}
						else
						{
							if constexpr (base < T(0))
							{
								if constexpr (exponent == std::numeric_limits<U>::infinity())
								{
									return -std::numeric_limits<T>::infinity();
								}
								else
								{
									if constexpr (exponent % 2 == 0)
									{
										return exp(exponent * ln(-base));
									}
									else
									{
										return -exp(exponent * ln(-base));
									}
								}
							}
							else if constexpr (base == T(0))
							{
								return T(0);
							}
							else // if base > T(0)
							{
								if constexpr (exponent == std::numeric_limits<U>::infinity())
								{
									return std::numeric_limits<T>::infinity();
								}
								else
								{
									return exp(exponent * ln(base));
								}
							}
						}
					}
				}
			} // pow 


		}   
	}
#endif