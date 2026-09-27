#pragma once
#include <concepts>
#include <cstdint>
#include <string>
template <typename T>
concept BinarySupportedType =
	std::same_as<T, double> || std::same_as<T, float> || std::same_as<T, std::uint8_t>
	|| std::same_as<T, std::uint16_t> || std::same_as<T, std::uint32_t>
	|| std::same_as<T, std::uint64_t> || std::same_as<T, int> || std::same_as<T, std::string>;