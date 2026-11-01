#pragma once
#include <karkinolution/functional/base_storage.hpp>
#include <karkinolution/organism/nature/grass/grass.hpp>
#include <karkinolution/organism/nature/identity.hpp>

namespace Karkinolution {
	namespace Organisms {
		namespace Nature {

			using NatureEntity = std::variant<Grass::Grass>;

			class NatureRegistry : public Functional::BaseStorage<NatureId, NatureEntity> {
				public:
					Grass::Grass &at_grass(NatureId id) {
						return std::get<Grass::Grass>(at(id));
					}
			};

			namespace NatureGetters {
				const Math::Geometry::Radius &get_radius(const NatureEntity &nature);
				const Vec3                   &get_position(const NatureEntity &nature);
			} // namespace NatureGetters

		} // namespace Nature
	} // namespace Organisms
} // namespace Karkinolution
