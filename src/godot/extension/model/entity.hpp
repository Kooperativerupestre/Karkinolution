#pragma once

#include "godot/extension/model/creature/creature.hpp"

#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/variant/variant.hpp>
#include <optional>
#include <variant>

using GodotEntity = std::variant<godot::Ref<GodotCreature>>;

namespace GodotEntityConversion {

	template <typename... Ts>
	inline std::optional<GodotEntity> from_variant(const godot::Variant &variant,
												   std::variant<Ts...>*) {
		std::optional<GodotEntity> result = std::nullopt;

		auto try_cast = [&]<typename T>() {
			if (result.has_value()) {
				return;
			}
			T casted = variant;
			if constexpr (requires { casted.is_valid(); }) {
				if (casted.is_valid()) {
					result = GodotEntity(casted);
				}
			}
		};

		(try_cast.template operator()<Ts>(), ...);
		return result;
	}

	inline std::optional<GodotEntity> to_entity(const godot::Variant &variant) {
		if (variant.get_type() != godot::Variant::OBJECT) {
			return std::nullopt;
		}
		return from_variant(variant, static_cast<GodotEntity*>(nullptr));
	}

} // namespace GodotEntityConversion
