#include "karkinolution/terrain/rtree/box.hpp"

#include <cassert>
#include <gtest/gtest.h>
#include <karkinolution/terrain/rtree/rtree.hpp>
#include <karkinolution/terrain/soil.hpp>
#include <karkinolution/utils/k_random.hpp>
#include <variant>

using TRStarTree  = RStarTree<SoilPieceId>;
using TRTreeNode  = RtreeNode<SoilPieceId>;
using TRTreeEntry = RtreeEntry<SoilPieceId>;

namespace {

Box3D make_box(double x, double y, double z, double size = 1.0) {
	return Box3D{.max = Vec3{x + size, y + size, z + size}, .min = Vec3{x, y, z}};
}

void assert_node_invariants(const TRTreeNode &node, bool is_root = true) {
	ASSERT_LE(node.entries.size(), TRStarTree::MAX_ENTRIES);

	if (!is_root) {
		ASSERT_GE(node.entries.size(), TRStarTree::MIN_ENTRIES);
	}

	if (node.type == NodeType::LEAF) {
		for (const TRTreeEntry &entry : node.entries) {
			ASSERT_TRUE(std::holds_alternative<SoilPieceId>(entry.content));
		}
	} else {
		for (const TRTreeEntry &entry : node.entries) {
			ASSERT_TRUE(std::holds_alternative<std::unique_ptr<TRTreeNode>>(entry.content));

			const auto &child = std::get<std::unique_ptr<TRTreeNode>>(entry.content);

			ASSERT_NE(child, nullptr);

			assert_node_invariants(*child, false);
		}
	}
}

void assert_box_equal(const Box3D &lhs, const Box3D &rhs) {
	EXPECT_DOUBLE_EQ(lhs.min.x, rhs.min.x);
	EXPECT_DOUBLE_EQ(lhs.min.y, rhs.min.y);
	EXPECT_DOUBLE_EQ(lhs.min.z, rhs.min.z);

	EXPECT_DOUBLE_EQ(lhs.max.x, rhs.max.x);
	EXPECT_DOUBLE_EQ(lhs.max.y, rhs.max.y);
	EXPECT_DOUBLE_EQ(lhs.max.z, rhs.max.z);
}

void assert_mbr_invariant(const TRTreeNode &node) {
	if (!node.entries.empty()) {
		Box3D expected = RStarTreeMotor::calculate_mbr(node.entries, 0, node.entries.size());

		ASSERT_TRUE(node.box.has_value());

		assert_box_equal(*node.box, expected);
	}

	if (node.type == NodeType::INTERNAL) {
		for (const TRTreeEntry &entry : node.entries) {
			const auto &child = std::get<std::unique_ptr<TRTreeNode>>(entry.content);

			assert_mbr_invariant(*child);
		}
	}
}

void assert_tree_invariants(const TRStarTree &tree) {
	assert_node_invariants(tree.root());
	assert_mbr_invariant(tree.root());
}

void assert_all_ids_exist(const TRStarTree        &tree,
						  SoilPieceId              count,
						  const std::vector<bool> &deleted = {}) {

	for (SoilPieceId id = 0; id < count; ++id) {
		if (!deleted.empty() && deleted[id]) {
			continue;
		}

		ASSERT_TRUE(tree.exists(id)) << "Lost SoilPieceId: " << id;
	}
}

} // namespace

TEST(RStarTreeTest, InvariantsHoldAfterMultipleInsertions) {
	TRStarTree tree;

	for (SoilPieceId id = 0; id < 1000; ++id) {
		tree.insert(id,
					make_box(static_cast<double>(id % 100),
							 static_cast<double>((id / 100) % 10),
							 static_cast<double>(static_cast<double>(id) / 1000)));

		if (!tree.exists(id)) {
			FAIL();
		}

		assert_tree_invariants(tree);
	}
}

TEST(RStarTreeTest, InvariantsHoldAfterMultipleInsertionsAndDeletions) {
	TRStarTree tree;

	constexpr SoilPieceId count = 1000;

	for (SoilPieceId id = 0; id < count; ++id) {
		tree.insert(id,
					make_box(static_cast<double>(id % 100),
							 static_cast<double>((id / 100) % 10),
							 static_cast<double>(static_cast<double>(id) / 1000)));

		if (!tree.exists(id)) {
			FAIL();
		}
	}

	assert_all_ids_exist(tree, count);
	assert_tree_invariants(tree);

	std::vector<bool> deleted(count, false);

	for (SoilPieceId id = 0; id < count; id += 2) {
		ASSERT_TRUE(tree.exists(id)) << "ID " << id << " does not exist BEFORE deletion";

		ASSERT_TRUE(tree.remove(id)) << "delete_soil_piece(" << id << ") returned false";

		ASSERT_FALSE(tree.exists(id)) << "ID " << id << " still exists AFTER deletion";

		deleted[id] = true;
		assert_all_ids_exist(tree, count, deleted);
		assert_tree_invariants(tree);
	}

	for (SoilPieceId id = 1; id < count; id += 2) {
		EXPECT_TRUE(tree.exists(id)) << "Odd ID disappeared: " << id;
	}
}

TEST(RStarTreeTest, DeleteNonexistentSoilPieceReturnsFalse) {
	TRStarTree tree;

	constexpr SoilPieceId id = 42;

	EXPECT_FALSE(tree.exists(id));
	EXPECT_FALSE(tree.remove(id));
	EXPECT_FALSE(tree.exists(id));

	assert_tree_invariants(tree);
}

TEST(RStarTreeTest, DeleteExistingSoilPieceReturnsTrueAndRemovesIt) {
	TRStarTree tree;

	constexpr SoilPieceId id = 42;

	tree.insert(id, make_box(0.0, 0.0, 0.0));

	ASSERT_TRUE(tree.exists(id));

	EXPECT_TRUE(tree.remove(id));
	EXPECT_FALSE(tree.exists(id));

	assert_tree_invariants(tree);
}

TEST(RStarTreeTest, DeletingSameSoilPieceTwiceReturnsFalse) {
	TRStarTree tree;

	constexpr SoilPieceId id = 42;

	tree.insert(id, make_box(0.0, 0.0, 0.0));

	ASSERT_TRUE(tree.remove(id));
	ASSERT_FALSE(tree.exists(id));

	EXPECT_FALSE(tree.remove(id));
	EXPECT_FALSE(tree.exists(id));

	assert_tree_invariants(tree);
}

TEST(RStarTreeTest, DeletingOneSoilPieceDoesNotRemoveOthers) {
	TRStarTree tree;

	constexpr SoilPieceId count = 100;

	for (SoilPieceId id = 0; id < count; ++id) {
		tree.insert(id, make_box(static_cast<double>(id * 2), 0.0, 0.0));
	}

	assert_all_ids_exist(tree, count);
	assert_tree_invariants(tree);

	constexpr SoilPieceId deleted_id = 50;

	ASSERT_TRUE(tree.exists(deleted_id));

	ASSERT_TRUE(tree.remove(deleted_id))
		<< "delete_soil_piece(" << deleted_id << ") returned false";

	EXPECT_FALSE(tree.exists(deleted_id));

	for (SoilPieceId id = 0; id < count; ++id) {
		if (id == deleted_id) {
			continue;
		}

		EXPECT_TRUE(tree.exists(id))
			<< "Lost soil piece after deleting " << deleted_id << ": " << id;
	}

	assert_tree_invariants(tree);
}
