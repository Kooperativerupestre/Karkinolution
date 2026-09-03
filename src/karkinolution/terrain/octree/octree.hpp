#pragma once
#include "karkinolution/math/physic/vec/model.hpp"

#include <karkinolution/terrain/octree/aabb.hpp>

namespace {
AABB world = {.max = {-1000, -1000, -1000}, .min = {1000, 1000, 1000}};
} // namespace

class Octree {
	private:

		std::unique_ptr<OctreeNode> root_;

	public:

		Octree()
			: root_(std::make_unique<OctreeNode>(OctreeNode{world})) {}

		OctreeNode &root() {
			return *root_;
		}

		const OctreeNode &root() const {
			return *root_;
		}

		double size() const {
			return world.volume();
		}

		const Vec3 &max() const {
			return root_->bounds().max;
		}

		const Vec3 &min() const {
			return root_->bounds().min;
		}
};
