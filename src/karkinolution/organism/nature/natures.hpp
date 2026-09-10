#pragma once
#include <karkinolution/core/basestorage.hpp>
#include <karkinolution/organism/nature/grass/grass.hpp>
#include <karkinolution/organism/nature/identity.hpp>

using Nature = std::variant<Grass>;

class NatureRegistry : public BaseStorage<NatureId, Nature> {
	public:

		Grass &at_grass(const NatureId &id) {
			return std::get<Grass>(at(id));
		}

		const Grass &at_grass(const NatureId &id) const {
			return std::get<Grass>(at(id));
		}
};

namespace NatureGetters {
	const GeometryForms::Radius &get_radius(const Nature &nature);
	const Vec3                  &get_position(const Nature &nature);
} // namespace NatureGetters