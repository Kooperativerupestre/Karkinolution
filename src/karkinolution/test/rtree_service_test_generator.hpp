#pragma once

#include <karkinolution/functional/base_storage.hpp>
#include <karkinolution/math/units.hpp>
#include <karkinolution/terrain/rtree/box_conversion.hpp>
#include <karkinolution/terrain/rtree/rtree.hpp>
#include <karkinolution/terrain/rtree/service.hpp>

// Don't use this if you aren't testing. It exists only for creating invalid states.
namespace Karkinolution::Terrain::Trees {
	class RSTServiceTest {
		public:

			template <typename ID, BoxConvertible V, typename Registry>
				requires Functional::BaseStorageChild<Registry, ID, V>
			static RSTService<ID, V, Registry>
			create(RTree::RStarTree<ID> &tree, Registry &registry, const Math::Units::Size &size) {
				RSTService<ID, V, Registry> service{size};
				service.tree_     = std::move(tree);
				service.registry_ = std::move(registry);
				return service;
			}

			template <typename ID, BoxConvertible V, typename Registry>
				requires Functional::BaseStorageChild<Registry, ID, V>
			static const RTree::RStarTree<ID> &
			get_tree(const RSTService<ID, V, Registry> &service) {
				return service.tree_;
			}

			template <typename ID, BoxConvertible V, typename Registry>
				requires Functional::BaseStorageChild<Registry, ID, V>
			static RTree::RStarTree<ID> &get_tree(RSTService<ID, V, Registry> &service) {
				return service.tree_;
			}

			template <typename ID, BoxConvertible V, typename Registry>
				requires Functional::BaseStorageChild<Registry, ID, V>
			static const Registry &get_registry(const RSTService<ID, V, Registry> &service) {
				return service.registry_;
			}

			template <typename ID, BoxConvertible V, typename Registry>
				requires Functional::BaseStorageChild<Registry, ID, V>
			static Registry &get_registry(RSTService<ID, V, Registry> &service) {
				return service.registry_;
			}

			template <typename ID, BoxConvertible V, typename Registry>
				requires Functional::BaseStorageChild<Registry, ID, V>
			static const Math::Units::Size &get_size(const RSTService<ID, V, Registry> &service) {
				return service.size_;
			}

			template <typename ID, BoxConvertible V, typename Registry>
				requires Functional::BaseStorageChild<Registry, ID, V>
			static Math::Units::Size &get_size(RSTService<ID, V, Registry> &service) {
				return service.size_;
			}
	};
} // namespace Karkinolution::Terrain::Trees

using Karkinolution::Terrain::Trees::RSTServiceTest;
