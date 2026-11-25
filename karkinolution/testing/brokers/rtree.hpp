#pragma once

#include <karkinolution/terrain/rtree/box_conversion.hpp>
#include <karkinolution/terrain/rtree/rtree.hpp>
#include <karkinolution/terrain/rtree/service.hpp>
#include <shared/math/units.hpp>
#include <unordered_map>

namespace Karkinolution::Testing::Brokers::RTree {

	class Access {
		public:

			template <typename ID, Terrain::Trees::BoxConvertible V>
			static Terrain::Trees::RSTService<ID, V> create(Terrain::Trees::RTree::RStarTree<ID> &tree,
															std::unordered_map<ID, V>            &registry,
															const KShared::Math::Units::Size     &size) {
				Terrain::Trees::RSTService<ID, V> service{size};
				service.tree_     = std::move(tree);
				service.registry_ = std::move(registry);
				return service;
			}

			template <typename ID, Terrain::Trees::BoxConvertible V>
			static const Terrain::Trees::RTree::RStarTree<ID> &
			get_tree(const Terrain::Trees::RSTService<ID, V> &service) {
				return service.tree_;
			}

			template <typename ID, Terrain::Trees::BoxConvertible V>
			static Terrain::Trees::RTree::RStarTree<ID> &get_tree(Terrain::Trees::RSTService<ID, V> &service) {
				return service.tree_;
			}

			template <typename ID, Terrain::Trees::BoxConvertible V>
			static const std::unordered_map<ID, V> &get_registry(const Terrain::Trees::RSTService<ID, V> &service) {
				return service.registry_;
			}

			template <typename ID, Terrain::Trees::BoxConvertible V>
			static std::unordered_map<ID, V> &get_registry(Terrain::Trees::RSTService<ID, V> &service) {
				return service.registry_;
			}

			template <typename ID, Terrain::Trees::BoxConvertible V>
			static const KShared::Math::Units::Size &get_size(const Terrain::Trees::RSTService<ID, V> &service) {
				return service.size_;
			}

			template <typename ID, Terrain::Trees::BoxConvertible V>
			static KShared::Math::Units::Size &get_size(Terrain::Trees::RSTService<ID, V> &service) {
				return service.size_;
			}
	};

} // namespace Karkinolution::Testing::Brokers::RTree
