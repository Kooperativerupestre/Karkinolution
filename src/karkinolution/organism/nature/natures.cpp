#include "karkinolution/math/geometry/models.hpp"
#include "karkinolution/math/physic/vec/model.hpp"

#include <karkinolution/functional/base_storage.hpp>
#include <karkinolution/organism/nature/grass/grass.hpp>
#include <karkinolution/organism/nature/identity.hpp>
#include <karkinolution/organism/nature/natures.hpp>
#include <utility>

namespace Karkinolution::Organisms::Nature {
	const Math::Geometry::Radius &NatureGetters::get_radius(const NatureEntity &nature_entity) {
		if (std::holds_alternative<Grass::Grass>(nature_entity)) {
			return std::get<Grass::Grass>(nature_entity).radius;
		}
		std::unreachable();
	}

	const Vec3 &NatureGetters::get_position(const NatureEntity &nature) {
		if (std::holds_alternative<Grass::Grass>(nature)) {
			return std::get<Grass::Grass>(nature).position;
		}
		std::unreachable();
	}
} // namespace Karkinolution::Organisms::Nature
