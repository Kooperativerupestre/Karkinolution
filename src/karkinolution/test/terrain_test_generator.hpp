#pragma once
#include "karkinolution/terrain/rtree/rtree.hpp"
#include "karkinolution/terrain/soil.hpp"

#include <karkinolution/math/units.hpp>
#include <karkinolution/terrain/terrain.hpp>

// don't include this if you aren't testing


class TerritoryTest {

		friend class Territory;

	public:

		TerritoryTest();

		static Territory
		create(SoilPieceRegistry &registry, RStarTree<SoilPieceId> &tree, const Size &size) {
			Territory territory{size};

			territory.data_  = std::move(tree);
			territory.soils_ = std::move(registry);
			return territory;
		}

		static const RStarTree<SoilPieceId> &get_tree(const Territory &territory) {
			return territory.data_;
		}

		static RStarTree<SoilPieceId> &get_tree(Territory &territory) {
			return territory.data_;
		}

		static const SoilPieceRegistry &get_registry(const Territory &territory) {
			return territory.soils_;
		}

		static SoilPieceRegistry &get_registry(Territory &territory) {
			return territory.soils_;
		}

		static const Size &get_size(const Territory &territory) {
			return territory.size_;
		}

		static Size &get_size(Territory &territory) {
			return territory.size_;
		}
};
