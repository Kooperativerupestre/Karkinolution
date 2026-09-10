#pragma once

#include <karkinolution/core/basestorage.hpp>
#include <karkinolution/math/physic/vec/model.hpp>
#include <karkinolution/math/physic/vec/utils.hpp>
#include <karkinolution/terrain/rtree/box_conversion.hpp>
#include <karkinolution/terrain/rtree/rtree.hpp>

// Don't use if you aren't testing.
// This exists only for creating invalid states.

class RSTServiceTest;

template <typename ID, BoxConvertible V, typename Registry>
	requires BaseStorageChild<Registry, ID, V>
class RSTService {
		friend class RSTServiceTest;

	private:

		RStarTree<ID> tree_;
		Registry      registry_;
		Size          size_;

	public:

		RSTService(const Size &size)
			: size_(size) {}

		const Registry &registry() const {
			return registry_;
		}

		const RStarTree<ID> &tree() const {
			return tree_;
		}

		const Size &size() const {
			return size_;
		}

		Box3D box() const {
			return BoxConversion::to_box(size_, Vec3Utils::zero());
		}

		bool add(ID id, V &&value) {
			const auto value_box = BoxConversion::to_box(value);

			if (!Box3DMotor::contains(box(), value_box)) {
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

		std::vector<ID> find(const Radius &radius, const Vec3 &position) const {
			auto box = BoxConversion::to_box(radius, position);
			return tree_.find(box);
		}

		std::vector<ID> find(const Size &size, const Vec3 &position) const {
			auto box = BoxConversion::to_box(size, position);
			return tree_.find(box);
		}

		void clear() {
			tree_.clear();
			registry_.clear();
		}
};