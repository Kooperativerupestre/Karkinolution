#pragma once
#include "karkinolution/terrain/rtree/rtree.hpp"
#include "karkinolution/terrain/rtree/service.hpp"

#include <karkinolution/math/units.hpp>
#include <karkinolution/terrain/rtree/box_conversion.hpp>
#include <karkinolution/terrain/rtree/rtree.hpp>

// don't include this if you aren't testing


class RSTServiceTest {


	public:

		RSTServiceTest();

		template <typename ID, BoxConvertible V, typename Registry>
			requires BaseStorageChild<Registry, ID, V>
		static RSTServiceTest


		create(RStarTree<ID> &tree, Registry &registry, const Size &size) {
			RSTService<ID, V, Registry> service{size};


			service.tree_     = std::move(tree);
			service.registry_ = std::move(registry);
			return service;
		}

		template <typename ID, BoxConvertible V, typename Registry>
			requires BaseStorageChild<Registry, ID, V>

		static const RStarTree<ID> &get_tree(const RSTService<ID, V, Registry> &service) {
			return service.data_;
		}

		template <typename ID, BoxConvertible V, typename Registry>
			requires BaseStorageChild<Registry, ID, V>

		static RStarTree<ID> &get_tree(RSTService<ID, V, Registry> &service) {
			return service.tree_;
		}

		template <typename ID, BoxConvertible V, typename Registry>
			requires BaseStorageChild<Registry, ID, V>

		static const Registry &get_registry(const RSTService<ID, V, Registry> &service) {
			return service.registry_;
		}

		template <typename ID, BoxConvertible V, typename Registry>
			requires BaseStorageChild<Registry, ID, V>
		static Registry &get_registry(RSTService<ID, V, Registry> &service) {
			return service.registry_;
		}

		template <typename ID, BoxConvertible V, typename Registry>
			requires BaseStorageChild<Registry, ID, V>
		static const Size &get_size(const RSTService<ID, V, Registry> &service) {
			return service.size_;
		}

		template <typename ID, BoxConvertible V, typename Registry>
			requires BaseStorageChild<Registry, ID, V>

		static Size &get_size(RSTService<ID, V, Registry> &service) {
			return service.size_;
		}
};
