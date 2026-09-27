#pragma once
#include "type_string.hpp"

#include <format>
#include <string>

namespace BinaryErrorFactory {

	constexpr std::string failed_conversion(const std::string &extra, const std::string &type) {
		return std::format("Error on bytes conversion (to {} type): {}", type, extra);
	}

	template <typename T> constexpr std::string type_is_not_supported(const std::string &extra) {
		return std::format("Type {} isn't supported: {}", nameof<T>(), extra);
	}
} // namespace BinaryErrorFactory