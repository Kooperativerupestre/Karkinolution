#pragma once
#include <cstdint>

template <std::size_t Offset, std::size_t Size> struct Field {
		static constexpr std::size_t OFFSET = Offset;
		static constexpr std::size_t SIZE   = Size;
		static constexpr std::size_t END    = Offset + Size;
};

template <typename Prev, std::size_t Size> using Next = Field<Prev::END, Size>;
