#pragma once

#include <shared/functional/data_structures.hpp>
#include <shared/math/units.hpp>
#include <karkinolution/terrain/rtree/box_conversion.hpp>
#include <karkinolution/terrain/rtree/rtree.hpp>
#include <karkinolution/terrain/rtree/service.hpp>
#include <unordered_map>

// Don't use this if you aren't testing. This exists only for creating invalid states.
namespace Karkinolution::Terrain::Trees {
	class RSTServiceTest {
		public:

			template <typename ID, BoxConvertible V>
			static RSTService<ID, V>
			create(RTree::RStarTree<ID> &tree, std::unordered_map<ID, V> &registry, const KShared::Math::Units::Size &size) {
				RSTService<ID, V> service{size};
				service.tree_     = std::move(tree);
				service.registry_ = std::move(registry);
				return service;
			}

			template <typename ID, BoxConvertible V>
			static const RTree::RStarTree<ID> &get_tree(const RSTService<ID, V> &service) {
				return service.tree_;
			}

			template <typename ID, BoxConvertible V> static RTree::RStarTree<ID> &get_tree(RSTService<ID, V> &service) {
				return service.tree_;
			}

			template <typename ID, BoxConvertible V>
			static const std::unordered_map<ID, V> &get_registry(const RSTService<ID, V> &service) {
				return service.registry_;
			}

			template <typename ID, BoxConvertible V>
			static std::unordered_map<ID, V> &get_registry(RSTService<ID, V> &service) {
				return service.registry_;
			}

			template <typename ID, BoxConvertible V>
			static const KShared::Math::Units::Size &get_size(const RSTService<ID, V> &service) {
				return service.size_;
			}

			template <typename ID, BoxConvertible V> static KShared::Math::Units::Size &get_size(RSTService<ID, V> &service) {
				return service.size_;
			}
	};
} // namespace Karkinolution::Terrain::Trees

using Karkinolution::Terrain::Trees::RSTServiceTest;
