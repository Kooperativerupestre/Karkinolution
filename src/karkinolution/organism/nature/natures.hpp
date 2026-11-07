#pragma once


#include "shared/functional/data_structures.hpp"

#include <karkinolution/organism/nature/grass/grass.hpp>
#include <karkinolution/organism/nature/identity.hpp>
#include <unordered_map>

namespace Karkinolution {
	namespace Organisms {
		namespace Nature {

			using NatureEntity = std::variant<Grass::Grass>;

			namespace NatureGetters {
				const KShared::Math::Geometry::Radius &get_radius(const NatureEntity &nature);
				const Vec2                            &get_position(const NatureEntity &nature);
				NatureId                               get_id(const NatureEntity &nature);
			} // namespace NatureGetters

			class NatureRegistry {
				private:

					std::unordered_map<NatureId, NatureEntity> data_;


				public:

					NatureEntity &at(NatureId id) {
						return data_.at(id);
					}

					const NatureEntity &at(NatureId id) const {
						return data_.at(id);
					}

					Grass::Grass &at_grass(NatureId id) {
						return std::get<Grass::Grass>(at(id));
					}

					const Grass::Grass &at_grass(NatureId id) const {
						return std::get<Grass::Grass>(at(id));
					}

					bool add(NatureEntity &&n_e) {
						return KShared::Functional::try_add(data_, NatureGetters::get_id(n_e), std::move(n_e));
					}
			};

		} // namespace Nature
	} // namespace Organisms
} // namespace Karkinolution
