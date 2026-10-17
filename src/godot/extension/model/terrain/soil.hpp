#pragma once

#include "godot/extension/model/math/geometry.hpp"
#include "godot/extension/model/properties.hpp"

#include <cstdint>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/vector3.hpp>
#include <optional>

namespace KarkinolutionGodot {
	namespace Models {
		namespace Soils {
			namespace Components {
				class GodotDamage : public godot::RefCounted {
						GDCLASS(GodotDamage, godot::RefCounted);

					public:

					protected:

						static void _bind_methods();

						float damage;

					public:

						GodotDamage() {}

						GodotDamage(float value)
							: damage(value) {}

						float get_value() const;

						void set_value(float value) {
							damage = value;
						}
				};

				class GodotMovementCost : public godot::RefCounted {
						GDCLASS(GodotMovementCost, godot::RefCounted);

					public:

					protected:

						float cost;

						static void _bind_methods();

					public:

						GodotMovementCost() {}

						GodotMovementCost(float value)
							: cost(value) {}

						[[nodiscard]] float get_value() const;

						void set_value(float value) {
							cost = value;
						}
				};
			} // namespace Components

			class GodotSoilPiece : public godot::RefCounted {
					GDCLASS(GodotSoilPiece, godot::RefCounted);

				public:

				protected:

					std::uint64_t                             id_{0};
					std::uint8_t                              type_{0};
					godot::Array                              properties_;
					godot::Array                              required_capabilities_;
					godot::Ref<Components::GodotDamage>       damage;
					godot::Ref<Components::GodotMovementCost> movement_cost;

					double         radius_{0.0};
					godot::Vector3 position_{0.0, 0.0, 0.0};

					static void _bind_methods();

				public:

					GodotSoilPiece();
					GodotSoilPiece(std::uint64_t                          id,
								   std::uint8_t                           type,
								   const godot::Array                    &properties,
								   const godot::Array                    &required_capabilities,
								   std::optional<Components::GodotDamage> damage,
								   std::optional<Components::GodotMovementCost> movement_cost,
								   double                                       radius,
								   const godot::Vector3                        &position);

					[[nodiscard]] std::uint64_t get_id() const;
					[[nodiscard]] std::uint8_t  get_type() const;
					[[nodiscard]] godot::String get_type_name() const;
					[[nodiscard]] godot::Array  get_properties() const;
					[[nodiscard]] godot::Array  get_required_capabilities() const;

					[[nodiscard]] double         get_radius() const;
					[[nodiscard]] godot::Vector3 get_position() const;
			};
		} // namespace Soils
	} // namespace Models
} // namespace KarkinolutionGodot