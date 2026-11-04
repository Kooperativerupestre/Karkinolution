#include <gtest/gtest.h>
#include <karkinolution/view/gui/geometry/geometry.hpp>

namespace Geometry = KarkinolutionView::Gui::Geometry;

TEST(GuiGeometry, ContainsSmallerCenteredRectangle) {
	const Geometry::Rectangle container{Geometry::Position{170, 320},
										Geometry::Width{140},
										Geometry::Height{40}};
	const Geometry::Rectangle contained{Geometry::Position{170, 320},
										Geometry::Width{130},
										Geometry::Height{20}};

	EXPECT_TRUE(Geometry::can_contain(container, contained));
}

TEST(GuiGeometry, ContainsRectangleTouchingEveryEdge) {
	const Geometry::Rectangle container{Geometry::Position{0, 0},
										Geometry::Width{100},
										Geometry::Height{60}};
	const Geometry::Rectangle contained{Geometry::Position{0, 0},
										Geometry::Width{100},
										Geometry::Height{60}};

	EXPECT_TRUE(Geometry::can_contain(container, contained));
}

TEST(GuiGeometry, RejectsRectangleExtendingPastAnyEdge) {
	const Geometry::Rectangle container{Geometry::Position{0, 0},
										Geometry::Width{100},
										Geometry::Height{100}};
	const Geometry::Rectangle too_high{Geometry::Position{0, -50},
									   Geometry::Width{10},
									   Geometry::Height{10}};
	const Geometry::Rectangle too_low{Geometry::Position{0, 50},
									  Geometry::Width{10},
									  Geometry::Height{10}};
	const Geometry::Rectangle too_far_left{Geometry::Position{-50, 0},
										   Geometry::Width{10},
										   Geometry::Height{10}};
	const Geometry::Rectangle too_far_right{Geometry::Position{50, 0},
											Geometry::Width{10},
											Geometry::Height{10}};

	EXPECT_FALSE(Geometry::can_contain(container, too_high));
	EXPECT_FALSE(Geometry::can_contain(container, too_low));
	EXPECT_FALSE(Geometry::can_contain(container, too_far_left));
	EXPECT_FALSE(Geometry::can_contain(container, too_far_right));
}

TEST(GuiGeometry, ContainsCircleOnBoundaryAndRejectsOverflow) {
	const Geometry::Rectangle container{Geometry::Position{0, 0},
										Geometry::Width{100},
										Geometry::Height{100}};
	const Geometry::Circle    contained{Geometry::Position{0, 0}, Geometry::Radius{50}};
	const Geometry::Circle    overflowing{Geometry::Position{1, 0}, Geometry::Radius{50}};

	EXPECT_TRUE(Geometry::can_contain(container, contained));
	EXPECT_FALSE(Geometry::can_contain(container, overflowing));
}
