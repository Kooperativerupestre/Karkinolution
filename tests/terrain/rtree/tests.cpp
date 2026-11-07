#include "karkinolution/terrain/rtree/box.hpp"

#include <cassert>
#include <gtest/gtest.h>
#include <karkinolution/terrain/rtree/rtree.hpp>
#include <karkinolution/terrain/soil/soil.hpp>
#include <shared/math/physic/vec/model.hpp>
#include <variant>

using Karkinolution::Terrain::Soil::SoilPieceId;
using Karkinolution::Terrain::Trees::RTree::Box2D;
using Karkinolution::Terrain::Trees::RTree::NodeType;
using Karkinolution::Terrain::Trees::RTree::RStarTree;
using Karkinolution::Terrain::Trees::RTree::RtreeEntry;
using Karkinolution::Terrain::Trees::RTree::RtreeNode;
using KShared::Math::Vec::Vec2;
namespace RStarTreeMotor = Karkinolution::Terrain::Trees::RTree::RStarTreeMotor;

using TRStarTree  = RStarTree<SoilPieceId>;
using TRTreeNode  = RtreeNode<SoilPieceId>;
using TRTreeEntry = RtreeEntry<SoilPieceId>;

namespace {

	Box2D make_box(double x, double y, double size = 1.0) {
		return Box2D{.max = Vec2{x + size, y + size}, .min = Vec2{x, y}};
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

	void assert_box_equal(const Box2D &lhs, const Box2D &rhs) {
		EXPECT_DOUBLE_EQ(lhs.min.x, rhs.min.x);
		EXPECT_DOUBLE_EQ(lhs.min.y, rhs.min.y);

		EXPECT_DOUBLE_EQ(lhs.max.x, rhs.max.x);
		EXPECT_DOUBLE_EQ(lhs.max.y, rhs.max.y);
	}

	void assert_mbr_invariant(const TRTreeNode &node) {
		if (!node.entries.empty()) {
			Box2D expected = RStarTreeMotor::calculate_mbr(node.entries, 0, node.entries.size());

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

	void assert_all_ids_exist(const TRStarTree &tree, SoilPieceId count, const std::vector<bool> &deleted = {}) {

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
		tree.insert(id, make_box(static_cast<double>(id % 100), static_cast<double>((id / 100) % 10)));

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
		tree.insert(id, make_box(static_cast<double>(id % 100), static_cast<double>((id / 100) % 10)));

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
		tree.insert(id, make_box(static_cast<double>(id * 2), 0.0));
	}

	assert_all_ids_exist(tree, count);
	assert_tree_invariants(tree);

	constexpr SoilPieceId deleted_id = 50;

	ASSERT_TRUE(tree.exists(deleted_id));

	ASSERT_TRUE(tree.remove(deleted_id)) << "delete_soil_piece(" << deleted_id << ") returned false";

	EXPECT_FALSE(tree.exists(deleted_id));

	for (SoilPieceId id = 0; id < count; ++id) {
		if (id == deleted_id) {
			continue;
		}

		EXPECT_TRUE(tree.exists(id)) << "Lost soil piece after deleting " << deleted_id << ": " << id;
	}
	assert_tree_invariants(tree);
}

TEST(RStarTreeTest, ClearReallyWorks) {
	TRStarTree tree;

	constexpr SoilPieceId count = 100;

	for (SoilPieceId id = 0; id < count; ++id) {
		tree.insert(id, make_box(static_cast<double>(id * 2), 0.0));
	}
	tree.clear();
	ASSERT_TRUE(tree.size() == 0);
}

TEST(RStarTreeTest, FindReallyWorks) {
	TRStarTree tree;

	constexpr SoilPieceId count = 100;

	// Insert 100 boxes in a 10 x 10 grid.
	for (SoilPieceId id = 0; id < count; ++id) {
		const double x = static_cast<double>(id % 10) * 10.0;
		const double y = static_cast<double>(id / 10) * 10.0;

		tree.insert(id, make_box(x, y));
	}

	assert_tree_invariants(tree);

	// Find a region containing exactly one box.
	{
		const Box2D query = make_box(20.0, 30.0, 1.0);

		const auto result = tree.find(query);

		ASSERT_EQ(result.size(), 1);
		EXPECT_EQ(result[0], 32);
	}

	// Find a larger region.
	//
	// Box boundaries are inclusive, so boxes starting exactly at x = 30 or y = 30
	// also intersect the query.
	//
	// x: 0, 10, 20, 30 -> 4
	// y: 0, 10, 20, 30 -> 4
	// Total: 4 * 4 = 16.
	{
		const Box2D query = Box2D{
			.max = Vec2{30.0, 30.0},
			.min = Vec2{0.0, 0.0},
		};

		const auto result = tree.find(query);

		ASSERT_EQ(result.size(), 16);

		std::unordered_set<SoilPieceId> expected;

		for (SoilPieceId y = 0; y <= 3; ++y) {
			for (SoilPieceId x = 0; x <= 3; ++x) {
				expected.insert(y * 10 + x);
			}
		}

		EXPECT_EQ(std::unordered_set<SoilPieceId>(result.begin(), result.end()), expected);
	}

	// Search a completely empty region.
	{
		const Box2D query = Box2D{
			.max = Vec2{1010.0, 1010.0},
			.min = Vec2{1000.0, 1000.0},
		};

		const auto result = tree.find(query);

		EXPECT_TRUE(result.empty());
	}

	// Search a region between the inserted boxes.
	{
		const Box2D query = Box2D{
			.max = Vec2{9.0, 9.0},
			.min = Vec2{5.0, 5.0},
		};

		const auto result = tree.find(query);

		EXPECT_TRUE(result.empty());
	}
}
