#pragma once

#include "karkinolution/core/id.hpp"
#include "karkinolution/math/geometry/models.hpp"
#include "karkinolution/math/physic/vec/model.hpp"
#include "karkinolution/organism/entities/properties/properties.hpp"
#include "karkinolution/terrain/soil/soil.hpp"

#include <optional>
#include <vector>

namespace Karkinolution {
	namespace Binary {
		namespace Interpreters {
			namespace Models {
				namespace Soil {
					struct DamageComponent {
							float value;
					};

					struct MovementCostComponent {
							float cost;
					};

					struct Soil {
							std::optional<DamageComponent>       damage_component;
							std::optional<MovementCostComponent> movement_cost_component;

							Karkinolution::Core::Id::BaseIdType                       id;
							Karkinolution::Math::Geometry::Radius                     radius;
							Karkinolution::Math::Vec::Vec2                            position;
							Karkinolution::Terrain::Soil::SoilTypes                   type;
							std::vector<Karkinolution::Terrain::Soil::SoilProperties> properties;
							std::vector<Karkinolution::Organisms::Entities::GenericProperty>
								required_properties;
					};


				} // namespace Soil
			} // namespace Models
		} // namespace Interpreters
	} // namespace Binary
} // namespace Karkinolution
