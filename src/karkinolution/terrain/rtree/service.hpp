#pragma once

#include <karkinolution/functional/base_storage.hpp>
#include <karkinolution/math/physic/vec/model.hpp>
#include <karkinolution/math/units.hpp>
#include <karkinolution/terrain/rtree/box_conversion.hpp>
#include <karkinolution/terrain/rtree/rtree.hpp>

// Don't use if you aren't testing.
// This exists only for creating invalid states.

namespace Karkinolution {
	namespace Terrain {
		namespace Trees {
			class RSTServiceTest;

			template <typename ID, BoxConvertible V, typename Registry>
				requires Functional::BaseStorageChild<Registry, ID, V>
			class RSTService {
					friend class RSTServiceTest;

				private:

					Karkinolution::Terrain::Trees::RTree::RStarTree<ID> tree_;
					Registry                                            registry_;
					Math::Units::Size                                   size_;

				public:

					RSTService(const Math::Units::Size &size)
						: size_(size) {}

					const Registry &registry() const {
						return registry_;
					}

					auto registry_values() {
						return registry_.values();
					}

					auto registry_values() const {
						return registry_.values();
					}

					const Karkinolution::Terrain::Trees::RTree::RStarTree<ID> &tree() const {
						return tree_;
					}

					const Math::Units::Size &size() const {
						return size_;
					}

					Karkinolution::Terrain::Trees::RTree::Box3D box() const {
						return RTree::BoxConversion::to_box(size_, Vec3{0.0, 0.0, 0.0});
					}

					bool add(ID id, V &&value) {
						const auto value_box = RTree::BoxConversion::to_box(value);

						if (!Karkinolution::Terrain::Trees::RTree::Box3DMotor::contains(
								box(),
								value_box)) {
							return false;
						}

						if (registry_.exists(id) || tree_.exists(id)) {
							return false;
						}

						bool added = registry_.try_add(id, std::move(value));
						if (!added) {
							return false;
						}

						tree_.insert(id, value_box);
						return true;
					}

					bool remove(ID id) {
						if (!tree_.exists(id) || !registry_.exists(id)) {
							return false;
						}

						auto was_removed = tree_.remove(id);
						if (!was_removed) {
							return false;
						}

						return registry_.try_del(id);
					}

					std::vector<ID> find(const Math::Geometry::Radius &radius,
										 const Vec3                   &position) const {
						auto box = RTree::BoxConversion::to_box(radius, position);
						return tree_.find(box);
					}

					std::vector<ID> find(const Math::Units::Size &size,
										 const Vec3              &position) const {
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
