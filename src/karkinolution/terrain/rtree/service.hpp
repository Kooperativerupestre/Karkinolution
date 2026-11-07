#pragma once

#include <shared/functional/data_structures.hpp>
#include <shared/math/physic/vec/model.hpp>
#include <shared/math/units.hpp>
#include <karkinolution/terrain/rtree/box_conversion.hpp>
#include <karkinolution/terrain/rtree/rtree.hpp>
#include <unordered_map>

// Don't use if you aren't testing.
// This exists only for creating invalid states.

namespace Karkinolution {
	namespace Terrain {
		namespace Trees {
			class RSTServiceTest;

			template <typename ID, BoxConvertible V> class RSTService {
					friend class RSTServiceTest;

				private:

					Karkinolution::Terrain::Trees::RTree::RStarTree<ID> tree_;
					std::unordered_map<ID, V>                           registry_;
					KShared::Math::Units::Size                                   size_;

				public:

					RSTService(const KShared::Math::Units::Size &size)
						: size_(size) {}

					std::unordered_map<ID, V> &registry() {
						return registry_;
					}

					const std::unordered_map<ID, V> &registry() const {
						return registry_;
					}

					const Karkinolution::Terrain::Trees::RTree::RStarTree<ID> &tree() const {
						return tree_;
					}

					const KShared::Math::Units::Size &size() const {
						return size_;
					}

					Karkinolution::Terrain::Trees::RTree::Box2D box() const {
						return RTree::BoxConversion::to_box(size_, Vec2{0.0, 0.0});
					}

					bool add(ID id, V &&value) {
						const auto value_box = RTree::BoxConversion::to_box(value);

						if (!Karkinolution::Terrain::Trees::RTree::Box2DMotor::contains(box(), value_box)) {
							return false;
						}

						if (registry_.contains(id) || tree_.exists(id)) {
							return false;
						}

						bool added = KShared::Functional::try_add(registry_, id, std::move(value));
						if (!added) {
							return false;
						}

						tree_.insert(id, value_box);
						return true;
					}

					bool remove(ID id) {
						if (!tree_.exists(id) || !registry_.contains(id)) {
							return false;
						}

						auto was_removed = tree_.remove(id);
						if (!was_removed) {
							return false;
						}

						return KShared::Functional::try_remove(registry_, id);
					}

					std::vector<ID> find(const KShared::Math::Geometry::Radius &radius, const Vec2 &position) const {
						auto box = RTree::BoxConversion::to_box(radius, position);
						return tree_.find(box);
					}

					std::vector<ID> find(const KShared::Math::Units::Size &size, const Vec2 &position) const {
						auto box = RTree::BoxConversion::to_box(size, position);
						return tree_.find(box);
					}

					void clear() {
						tree_.clear();
						registry_.clear();
					}
			};

		} // namespace Trees
	} // namespace Terrain
} // namespace Karkinolution
