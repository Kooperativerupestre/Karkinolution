#pragma once
#include <format>
#include <string>

namespace BinaryErrorFactory {

	constexpr std::string failed_conversion(const std::string &extra, const std::string &type) {
		return std::format("Error on bytes conversion (to {} type): {}", type, extra);
	}
} // namespace BinaryErrorFactory