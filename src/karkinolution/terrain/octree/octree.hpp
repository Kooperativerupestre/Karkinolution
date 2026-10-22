#pragma once
#include "karkinolution/math/physic/vec/model.hpp"

#include <karkinolution/math/units.hpp>
#include <karkinolution/terrain/octree/aabb.hpp>

class Octree {
	private:

		std::unique_ptr<OctreeNode> root_;
		Size                        size_;

	public:

		Octree(const Size &size)
			: root_(std::make_unique<OctreeNode>(
				  OctreeNode{AABBConversion::to_aabb(size, Vec3{0.0, 0.0, 0.0})}))
			, size_(size) {}

		OctreeNode &root() {
			return *root_;
		}

		const OctreeNode &root() const {
			return *root_;
		}

		Volume volume() const {
			return size_.volume();
		}

		const Vec3 &max() const {
			return root_->bounds().max;
		}

		const Vec3 &min() const {
			return root_->bounds().min;
		}
};
