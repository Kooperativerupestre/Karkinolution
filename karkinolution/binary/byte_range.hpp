#pragma once
#include <bit>
#include <concepts>
#include <cstdint>

namespace Karkinolution::Binary {

	template <typename T>
	concept ByteRange = requires(const T &buffer, std::size_t offset) {
		{ buffer.size() } -> std::convertible_to<std::size_t>;
		{ buffer[offset] } -> std::convertible_to<std::byte>;
	};

} // namespace Karkinolution::Binary