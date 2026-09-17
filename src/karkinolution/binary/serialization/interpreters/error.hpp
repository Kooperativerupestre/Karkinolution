#pragma once
#include <string>
#include <vector>

namespace ErrorSRI {
	inline constexpr size_t ERROR_SIZE = 360;

	std::vector<std::byte> serialize_error(const std::string &error);

} // namespace ErrorSRI