#ifndef Array2D_hxx
    #define Array2D_hxx

    #include <array>

    namespace Utils
    {
        template <typename T, std::size_t SizeX, std::size_t SizeY>
        class Array2D
        {
            private:
                std::array<T,SizeX * SizeY> m_data;

            public:
                using value_type = T;

                constexpr auto begin() noexcept {return m_data.begin();}
                constexpr auto end() noexcept {return m_data.end();}
                constexpr const auto begin() const noexcept {return m_data.begin();}
                constexpr const auto end() const noexcept {return m_data.end();}
                [[nodiscard]] constexpr T& at(std::size_t xIndex, std::size_t yIndex) noexcept {return m_data[xIndex + yIndex * SizeX];}
                [[nodiscard]] constexpr const T& at(std::size_t xIndex, std::size_t yIndex) const noexcept {return m_data[xIndex + yIndex * SizeX];}
        };
    }

#endif