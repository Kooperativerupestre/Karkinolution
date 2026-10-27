#pragma once

#include <concepts>
#include <functional>
#include <ranges>

namespace Karkinolution {
	namespace Binary {
		namespace Types {
			template <typename F, typename T>
			concept SerializerFunctionFor = requires(F f, T value) {
				{ std::invoke(f, value) } -> std::same_as<std::vector<std::byte>>;
			};
			template <typename F, typename T>
			concept DeserializerFunctionFor =
				requires(F f, const std::vector<std::byte> &bytes, std::size_t offset) {
					{ std::invoke(f, bytes, offset) } -> std::same_as<T>;
				};
		} // namespace Types
	} // namespace Binary
} // namespace Karkinolution