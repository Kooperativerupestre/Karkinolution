#include "karkinolution/math/geometry/models.hpp"
#include "karkinolution/math/physic/vec/model.hpp"

#include <karkinolution/core/basestorage.hpp>
#include <karkinolution/organism/nature/grass/grass.hpp>
#include <karkinolution/organism/nature/identity.hpp>
#include <karkinolution/organism/nature/natures.hpp>
#include <utility>

const GeometryForms::Radius &NatureGetters::get_radius(const Nature &nature) {
	if (std::holds_alternative<Grass>(nature)) {
		return std::get<Grass>(nature).radius;
	}
	std::unreachable();
}

const Vec3 &NatureGetters::get_position(const Nature &nature) {
	if (std::holds_alternative<Grass>(nature)) {
		return std::get<Grass>(nature).position;
	}
	std::unreachable();
}
