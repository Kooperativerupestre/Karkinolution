#include "shared/math/geometry/models.hpp"
#include "shared/math/physic/vec/model.hpp"

#include <karkinolution/organism/nature/grass/grass.hpp>
#include <karkinolution/organism/nature/identity.hpp>
#include <karkinolution/organism/nature/natures.hpp>
#include <utility>
#include <variant>

namespace Karkinolution::Organisms::Nature {
	const KShared::Math::Geometry::Radius &NatureGetters::get_radius(const NatureEntity &nature_entity) {
		if (std::holds_alternative<Grass::Grass>(nature_entity)) {
			return std::get<Grass::Grass>(nature_entity).radius;
		}
		std::unreachable();
	}

	const Vec2 &NatureGetters::get_position(const NatureEntity &nature) {
		if (std::holds_alternative<Grass::Grass>(nature)) {
			return std::get<Grass::Grass>(nature).position;
		}
		std::unreachable();
	}

	NatureId NatureGetters::get_id(const NatureEntity &nature) {
		if (std::holds_alternative<Grass::Grass>(nature)) {
			return std::get<Grass::Grass>(nature).build_id();
		}
		std::unreachable();
	}
} // namespace Karkinolution::Organisms::Nature
