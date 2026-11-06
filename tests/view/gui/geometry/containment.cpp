#include <gtest/gtest.h>
#include <karkinolution/view/gui/geometry/geometry.hpp>

namespace Geometry = KarkinolutionView::Gui::Geometry;

TEST(GuiGeometry, ContainsSmallerCenteredRectangle) {
	const Geometry::Rectangle container{Geometry::Position{170, 320}, Geometry::Width{140}, Geometry::Height{40}};
	const Geometry::Rectangle contained{Geometry::Position{170, 320}, Geometry::Width{130}, Geometry::Height{20}};

	EXPECT_TRUE(Geometry::can_contain(container, contained));
}

TEST(GuiGeometry, ContainsRectangleTouchingEveryEdge) {
	const Geometry::Rectangle container{Geometry::Position{0, 0}, Geometry::Width{100}, Geometry::Height{60}};
	const Geometry::Rectangle contained{Geometry::Position{0, 0}, Geometry::Width{100}, Geometry::Height{60}};

	EXPECT_TRUE(Geometry::can_contain(container, contained));
}

TEST(GuiGeometry, RejectsRectangleExtendingPastAnyEdge) {
	const Geometry::Rectangle container{Geometry::Position{0, 0}, Geometry::Width{100}, Geometry::Height{100}};
	const Geometry::Rectangle too_high{Geometry::Position{0, -50}, Geometry::Width{10}, Geometry::Height{10}};
	const Geometry::Rectangle too_low{Geometry::Position{0, 50}, Geometry::Width{10}, Geometry::Height{10}};
	const Geometry::Rectangle too_far_left{Geometry::Position{-50, 0}, Geometry::Width{10}, Geometry::Height{10}};
	const Geometry::Rectangle too_far_right{Geometry::Position{50, 0}, Geometry::Width{10}, Geometry::Height{10}};

	EXPECT_FALSE(Geometry::can_contain(container, too_high));
	EXPECT_FALSE(Geometry::can_contain(container, too_low));
	EXPECT_FALSE(Geometry::can_contain(container, too_far_left));
	EXPECT_FALSE(Geometry::can_contain(container, too_far_right));
}

TEST(GuiGeometry, ContainsCircleOnBoundaryAndRejectsOverflow) {
	const Geometry::Rectangle container{Geometry::Position{0, 0}, Geometry::Width{100}, Geometry::Height{100}};
	const Geometry::Circle    contained{Geometry::Position{0, 0}, Geometry::Radius{50}};
	const Geometry::Circle    overflowing{Geometry::Position{1, 0}, Geometry::Radius{50}};

	EXPECT_TRUE(Geometry::can_contain(container, contained));
	EXPECT_FALSE(Geometry::can_contain(container, overflowing));
}

TEST(GuiGeometry, BuildsRectangleFromEdges) {
	const auto rectangle = Geometry::Rectangle::from_edges(Geometry::Left{-20},
											 Geometry::Top{10},
											 Geometry::Right{40},
											 Geometry::Floor{70});

	EXPECT_EQ(rectangle.position, (Geometry::Position{10, 40}));
	EXPECT_DOUBLE_EQ(rectangle.width.value(), 60.0);
	EXPECT_DOUBLE_EQ(rectangle.height.value(), 60.0);
	EXPECT_FLOAT_EQ(rectangle.left().value, -20.0F);
	EXPECT_FLOAT_EQ(rectangle.right().value, 40.0F);
	EXPECT_FLOAT_EQ(rectangle.top().value, 10.0F);
	EXPECT_FLOAT_EQ(rectangle.floor().value, 70.0F);
}

TEST(GuiGeometry, RectangleFromEdgesRoundTripsCenterAndExtents) {
	const Geometry::Rectangle original{Geometry::Position{17.5F, 23.5F},
											 Geometry::Width{31.0},
											 Geometry::Height{19.0}};
	const auto reconstructed = Geometry::Rectangle::from_edges(original.left(),
															 original.top(),
															 original.right(),
															 original.floor());

	EXPECT_EQ(reconstructed, original);
}
