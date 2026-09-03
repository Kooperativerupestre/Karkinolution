#pragma once
#include <cmath>

namespace MathF {
	constexpr double sin(double x) {
		return std::sin(x);
	}

	constexpr double cos(double x) {
		return std::cos(x);
	}

	constexpr double tan(double x) {
		return std::tan(x);
	}

	constexpr double asin(double x) {
		return std::asin(x);
	}

	constexpr double acos(double x) {
		return std::acos(x);
	}

	constexpr double atan(double x) {
		return std::atan(x);
	}

	constexpr double atan2(double y, double x) {
		return std::atan2(y, x);
	}

	constexpr double sqrt(double x) {
		return std::sqrt(x);
	}

	constexpr double cbrt(double x) {
		return std::cbrt(x);
	}

	constexpr double pow(double x, double y) {
		return std::pow(x, y);
	}

	constexpr double exp(double x) {
		return std::exp(x);
	}

	constexpr double log(double x) {
		return std::log(x);
	}

	constexpr double log2(double x) {
		return std::log2(x);
	}

	constexpr double log10(double x) {
		return std::log10(x);
	}

	constexpr double floor(double x) {
		return std::floor(x);
	}

	constexpr double ceil(double x) {
		return std::ceil(x);
	}

	constexpr double round(double x) {
		return std::round(x);
	}

	constexpr double abs(double x) {
		return std::abs(x);
	}

	constexpr double min(double x, double y) {
		return std::min(x, y);
	}

	constexpr double max(double x, double y) {
		return std::max(x, y);
	}
} // namespace MathF