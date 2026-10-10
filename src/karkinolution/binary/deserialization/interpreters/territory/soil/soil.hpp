#pragma once
#include "karkinolution/core/id.hpp"
#include "karkinolution/organism/entities/properties/properties.hpp"
#include "karkinolution/terrain/soil.hpp"

#include <karkinolution/binary/serialization/interpreters/territory/soil/alias.hpp>
#include <optional>

namespace Karkinolution {
	namespace Binary {
		namespace Interpreters {
			namespace DSI {
				namespace Soil {
					namespace Models {

						struct DamageComponent {
								float value;
						};

						struct MovementCostComponent {
								float cost;
						};

						struct Soil {
								std::optional<DamageComponent>       damage_component;
								std::optional<MovementCostComponent> movement_cost_component;

								BaseIdType                   id;
								GeometryForms::Radius        radius;
								Vec3                         position;
								SoilTypes                    type;
								std::vector<SoilProperties>  properties;
								std::vector<GenericProperty> required_properties;
						};
					} // namespace Models

					std::optional<Models::DamageComponent>
					get_damage(const std::vector<std::byte> &payload, std::size_t offset);
					std::optional<Models::MovementCostComponent>
					get_movement_cost(const std::vector<std::byte> &payload, std::size_t offset);
					BaseIdType get_id(const std::vector<std::byte> &payload, std::size_t offset);
					Vec3 get_position(const std::vector<std::byte> &payload, std::size_t offset);
					SoilTypes get_type(const std::vector<std::byte> &payload, std::size_t offset);
					Radius    get_radius(const std::vector<std::byte> &payload, std::size_t offset);

					std::vector<SoilProperties>
					get_soil_properties(const std::vector<std::byte> &payload, std::size_t offset);
					std::vector<GenericProperty>
					get_required_properties(const std::vector<std::byte> &payload,
											std::size_t                   offset);

					Models::Soil get_soil(const std::vector<std::byte> &payload,
										  std::size_t                   offset);
				} // namespace Soil
			} // namespace DSI
		} // namespace Interpreters
	} // namespace Binary
} // namespace Karkinolution
