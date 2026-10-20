#pragma once

#include <magic_enum/magic_enum.hpp>

#include <cstdint>
#include <string>
#include <type_traits>
#include <vector>

namespace KarkinolutionGodot::Reflection {

	struct EnumConstant {
		std::string name;
		std::int64_t value;

		friend bool operator==(const EnumConstant &, const EnumConstant &) = default;
	};

	// Builds a runtime-friendly view of an enum without maintaining a second
	// list of its names or values. This helper is also tested independently of
	// Godot, so reflection mistakes can be caught by the normal GoogleTest suite.
	template <typename Enum>
	[[nodiscard]] std::vector<EnumConstant> enum_constants() {
		static_assert(std::is_enum_v<Enum>, "enum_constants requires an enum type");

		std::vector<EnumConstant> constants;
		constexpr auto entries = magic_enum::enum_entries<Enum>();
		constants.reserve(entries.size());

		for (const auto &[value, name] : entries) {
			constants.push_back(EnumConstant{
					std::string(name),
					static_cast<std::int64_t>(value),
			});
		}

		return constants;
	}

} // namespace KarkinolutionGodot::Reflection
