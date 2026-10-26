#pragma once

#include "karkinolution/core/id.hpp"
#include "karkinolution/math/geometry/models.hpp"
#include "karkinolution/math/physic/vec/model.hpp"
#include "karkinolution/organism/entities/properties/properties.hpp"
#include "karkinolution/terrain/soil.hpp"

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

							BaseIdType                   id;
							GeometryForms::Radius        radius;
							Vec3                         position;
							SoilTypes                    type;
							std::vector<SoilProperties>  properties;
							std::vector<GenericProperty> required_properties;
					};


				} // namespace Soil
			} // namespace Models
		} // namespace Interpreters
	} // namespace Binary
} // namespace Karkinolution
