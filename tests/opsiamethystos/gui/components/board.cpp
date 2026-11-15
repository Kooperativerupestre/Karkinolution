#include <gtest/gtest.h>
#include <opsiamethystos/gui/components/board.hpp>
#include <shared/core/color.hpp>

namespace {
	namespace Geometry   = Opsiamethystos::Gui::Geometry;
	namespace Components = Opsiamethystos::Gui::Components;
	using KShared::Colors::Presets::White;

	Components::Board board() {
		return Components::Board{Geometry::Rectangle{{0, 0}, Geometry::Width{100}, Geometry::Height{100}}};
	}
} // namespace

TEST(Board, AcceptsOverlappingPrimitives) {
	auto target = board();

	EXPECT_TRUE(target.draw_rectangle(Geometry::Rectangle{{0, 0}, Geometry::Width{60}, Geometry::Height{60}}, White));
	EXPECT_TRUE(target.draw_circle(Geometry::Circle{{10, 10}, Geometry::Radius{20}}, White));
	EXPECT_EQ(target.size(), 2);
}

TEST(Board, RejectsRectangleOutsideItsBounds) {
	auto target = board();

	EXPECT_FALSE(target.draw_rectangle(Geometry::Rectangle{{46, 0}, Geometry::Width{10}, Geometry::Height{10}}, White));
	EXPECT_EQ(target.size(), 0);
}

TEST(Board, RejectsCircleOutsideItsBounds) {
	auto target = board();

	EXPECT_FALSE(target.draw_circle(Geometry::Circle{{0, 46}, Geometry::Radius{5}}, White));
	EXPECT_EQ(target.size(), 0);
}

TEST(Board, AcceptsPrimitivesTouchingItsBoundary) {
	auto target = board();

	EXPECT_TRUE(target.draw_rectangle(Geometry::Rectangle{{45, 0}, Geometry::Width{10}, Geometry::Height{10}}, White));
	EXPECT_TRUE(target.draw_circle(Geometry::Circle{{0, -45}, Geometry::Radius{5}}, White));
	EXPECT_EQ(target.size(), 2);
}
