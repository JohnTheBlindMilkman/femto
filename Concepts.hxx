#ifndef Concepts_hxx
    #define Concepts_hxx

    #include <type_traits>
    #include <concepts>

    template <typename T>
    concept Arithmetic = std::integral<std::remove_cvref_t<T> > || std::floating_point<std::remove_cvref_t<T> >;

    template <typename T, typename ... Args>
    concept Functionlike = std::invocable<T,Args...> && requires(T func, Args &&... args)
    {
        {func(std::forward<Args>(args)...)} -> Arithmetic;
    };

#endif