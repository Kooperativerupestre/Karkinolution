#include "karkinolution/view/gui/panel/quadtree/quadtree.hpp"

#include <algorithm>
#include <gtest/gtest.h>

namespace {
	namespace Geometry = KarkinolutionView::Gui::Geometry;
	namespace Quadtree = KarkinolutionView::Gui::Panel::Quadtree;
	using KarkinolutionView::Gui::Components::ComponentType;
	using KarkinolutionView::Gui::Components::gen_id;

	Geometry::Rectangle rectangle(float x, float y, double width = 10, double height = 10) {
		return {Geometry::Position{x, y}, Geometry::Width{width}, Geometry::Height{height}};
	}
} // namespace

TEST(Quadtree, AddAndRemoveMaintainExistenceAndSize) {
	Quadtree::Quadtree tree{rectangle(0, 0, 1000, 1000)};
	const auto         id   = gen_id(ComponentType::GRID);
	auto               form = rectangle(-300, -300);

	EXPECT_TRUE(tree.add(id, form));
	EXPECT_FALSE(tree.add(id, rectangle(300, 300)));
	EXPECT_EQ(tree.size(), 1);
	EXPECT_TRUE(tree.exists(id));
	EXPECT_TRUE(tree.exists(id, form));

	EXPECT_TRUE(tree.remove(id, form));
	EXPECT_FALSE(tree.exists(id));
	EXPECT_EQ(tree.size(), 0);
	EXPECT_FALSE(tree.remove(id));
}

TEST(Quadtree, RemovalOfMissingIdsDoesNotChangeTheTree) {
	Quadtree::Quadtree tree{rectangle(0, 0, 1000, 1000)};
	const auto         existing_id = gen_id(ComponentType::GRID);
	const auto         missing_id  = gen_id(ComponentType::GRID);
	const auto         form        = rectangle(-300, -300);

	ASSERT_TRUE(tree.add(existing_id, form));
	EXPECT_FALSE(tree.remove(missing_id));
	EXPECT_FALSE(tree.remove(missing_id, form));
	EXPECT_TRUE(tree.exists(existing_id));
	EXPECT_EQ(tree.size(), 1);
}

TEST(Quadtree, OldBoundsNarrowExistenceAndRemovalToTheMatchingQuadrant) {
	Quadtree::Quadtree tree{rectangle(0, 0, 1000, 1000)};
	const auto         id       = gen_id(ComponentType::GRID);
	auto               old_form = rectangle(-300, -300);
	auto               wrong    = rectangle(300, 300);

	ASSERT_TRUE(tree.add(id, old_form));
	for (const auto position : {Geometry::Position{-300, 300},
								Geometry::Position{300, -300},
								Geometry::Position{300, 300},
								Geometry::Position{-200, 300},
								Geometry::Position{300, -200},
								Geometry::Position{200, 300},
								Geometry::Position{300, 200},
								Geometry::Position{-200, -300},
								Geometry::Position{-300, -200}}) {
		ASSERT_TRUE(tree.add(gen_id(ComponentType::GRID), rectangle(position.x, position.y)));
	}

	EXPECT_FALSE(tree.exists(id, wrong));
	EXPECT_FALSE(tree.remove(id, wrong));
	EXPECT_TRUE(tree.exists(id, old_form));
}

TEST(Quadtree, UpdateMovesEntryAndKeepsSizeConsistent) {
	Quadtree::Quadtree tree{rectangle(0, 0, 1000, 1000)};
	const auto         id       = gen_id(ComponentType::GRID);
	auto               old_form = rectangle(-300, -300);
	auto               new_form = rectangle(300, 300);

	ASSERT_TRUE(tree.add(id, old_form));
	for (int index = 0; index < 9; ++index) {
		ASSERT_TRUE(tree.add(gen_id(ComponentType::GRID), rectangle(300 + index * 10, 300)));
	}
	EXPECT_EQ(tree.update(id, new_form, old_form), Quadtree::UpdateOutput::UPDATED);
	EXPECT_EQ(tree.size(), 10);
	EXPECT_FALSE(tree.exists(id, old_form));
	EXPECT_TRUE(tree.exists(id, new_form));

	const auto outside = rectangle(600, 0);
	EXPECT_EQ(tree.update(id, outside), Quadtree::UpdateOutput::DOES_NOT_EXIST);
	EXPECT_TRUE(tree.exists(id, new_form));
	EXPECT_EQ(tree.size(), 10);
	EXPECT_TRUE(tree.invariants_hold());
}

TEST(Quadtree, FindReturnsIntersectingBoundsAndOneMatchingCandidate) {
	Quadtree::Quadtree tree{rectangle(0, 0, 1000, 1000)};
	const auto         left_id  = gen_id(ComponentType::GRID);
	const auto         hit_id   = gen_id(ComponentType::GRID);
	const auto         edge_id  = gen_id(ComponentType::GRID);
	const auto         right_id = gen_id(ComponentType::GRID);

	ASSERT_TRUE(tree.add(left_id, rectangle(-100, -100)));
	ASSERT_TRUE(tree.add(hit_id, rectangle(10, -100)));
	ASSERT_TRUE(tree.add(edge_id, rectangle(25, -100)));
	ASSERT_TRUE(tree.add(right_id, rectangle(100, -100)));
	for (int index = 0; index < 9; ++index) {
		ASSERT_TRUE(tree.add(gen_id(ComponentType::GRID), rectangle(-300 + index * 10, 300)));
	}

	const auto results = tree.find_in_area(rectangle(0, -100, 40, 20));
	ASSERT_EQ(results.size(), 2);
	EXPECT_NE(std::find(results.begin(), results.end(), hit_id), results.end());
	EXPECT_NE(std::find(results.begin(), results.end(), edge_id), results.end());
	EXPECT_TRUE(tree.if_find_one_stop(rectangle(0, -100, 40, 20)).has_value());
	EXPECT_TRUE(tree.find_in_area(rectangle(0, 100, 2, 2)).empty());
	EXPECT_FALSE(tree.if_find_one_stop(rectangle(0, 100, 2, 2)).has_value());
}

TEST(Quadtree, CircleBoundsAreIndexedForSpatialQueries) {
	Quadtree::Quadtree     tree{rectangle(0, 0, 1000, 1000)};
	const auto             id = gen_id(ComponentType::CIRCLE);
	const Geometry::Circle form{Geometry::Position{100, 100}, Geometry::Radius{20}};

	EXPECT_TRUE(tree.add(id, form));
	EXPECT_FALSE(tree.add(id, form));
	EXPECT_EQ(tree.size(), 1);
	EXPECT_TRUE(tree.exists(id));
	EXPECT_EQ(tree.find_in_area(rectangle(120, 100, 2, 2)).size(), 1);
	EXPECT_EQ(tree.find_in_area(Geometry::Circle{Geometry::Position{120, 100}, Geometry::Radius{5}})
				  .size(),
			  1);
	EXPECT_TRUE(
		tree.find_in_area(Geometry::Circle{Geometry::Position{150, 100}, Geometry::Radius{5}})
			.empty());
}

TEST(Quadtree, AddAndUpdatePreserveStructuralInvariants) {
	Quadtree::Quadtree tree{rectangle(0, 0, 1000, 1000)};

	for (int row = 0; row < 10; ++row) {
		for (int column = 0; column < 10; ++column) {
			const auto form = rectangle(-475 + column * 100, -475 + row * 100, 8, 8);
			ASSERT_TRUE(tree.add(gen_id(ComponentType::GRID), form));
		}
	}
	EXPECT_EQ(tree.size(), 100);
	EXPECT_TRUE(tree.invariants_hold());

	const auto id       = gen_id(ComponentType::GRID);
	const auto old_form = rectangle(-475, -475, 8, 8);
	const auto new_form = rectangle(425, 425, 8, 8);
	ASSERT_TRUE(tree.add(id, old_form));
	ASSERT_EQ(tree.update(id, new_form, old_form), Quadtree::UpdateOutput::UPDATED);
	EXPECT_EQ(tree.size(), 101);
	EXPECT_TRUE(tree.invariants_hold());
}
