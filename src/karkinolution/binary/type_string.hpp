#pragma once
#include <string>

template <typename T> constexpr std::string_view nameof() {
	if constexpr (std::is_same_v<T, bool>) {
		return "bool";
	} else if constexpr (std::is_same_v<T, char>) {
		return "char";
	} else if constexpr (std::is_same_v<T, signed char>) {
		return "signed char";
	} else if constexpr (std::is_same_v<T, unsigned char>) {
		return "unsigned char";
	} else if constexpr (std::is_same_v<T, short>) {
		return "short";
	} else if constexpr (std::is_same_v<T, unsigned short>) {
		return "unsigned short";
	} else if constexpr (std::is_same_v<T, int>) {
		return "int";
	} else if constexpr (std::is_same_v<T, unsigned int>) {
		return "unsigned int";
	} else if constexpr (std::is_same_v<T, long>) {
		return "long";
	} else if constexpr (std::is_same_v<T, unsigned long>) {
		return "unsigned long";
	} else if constexpr (std::is_same_v<T, long long>) {
		return "long long";
	} else if constexpr (std::is_same_v<T, unsigned long long>) {
		return "unsigned long long";
	} else if constexpr (std::is_same_v<T, float>) {
		return "float";
	} else if constexpr (std::is_same_v<T, double>) {
		return "double";
	} else if constexpr (std::is_same_v<T, long double>) {
		return "long double";
	} else {
		return "unknown";
	}
}