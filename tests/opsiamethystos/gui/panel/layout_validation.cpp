#include <cmath>
#include <gtest/gtest.h>
#include <limits>
#include <memory>
#include <opsiamethystos/graphics/panel/layout/layout.hpp>
#include <stdexcept>
#include <vector>

namespace Layout = Opsiamethystos::Gui::Panel::Layout;
using Opsiamethystos::Gui::Components::ComponentType;
using Opsiamethystos::Gui::Components::gen_id;

namespace {
	constexpr double not_a_number = std::numeric_limits<double>::quiet_NaN();
	constexpr double infinity     = std::numeric_limits<double>::infinity();

	Layout::Child fresh_leaf_child(const Layout::SizePolicy &policy = Layout::Fixed{10}) {
		return {policy,
				std::make_shared<const Layout::Node>(Layout::Node{Layout::Leaf{gen_id(ComponentType::RECTANGLE), {}}})};
	}

	Layout::Node
	stack_with(std::vector<Layout::Child> children, const double spacing = 0.0, const Layout::Insets padding = {}) {
		return Layout::Node{Layout::Stack{.direction = Layout::Direction::HORIZONTAL,
										  .padding   = padding,
										  .spacing   = spacing,
										  .children  = std::move(children)}};
	}
} // namespace

TEST(GuiLayoutValidation, AcceptsTheBoundaryValuesOfEveryField) {
	EXPECT_NO_THROW(Layout::validate(stack_with({})));
	EXPECT_NO_THROW(Layout::validate(Layout::Node{Layout::Leaf{gen_id(ComponentType::RECTANGLE), {}}}));
	EXPECT_NO_THROW(Layout::validate(stack_with(
		{fresh_leaf_child(Layout::Fixed{1}), fresh_leaf_child(Layout::Fill{0.0}), fresh_leaf_child(Layout::Content{})},
		0.0,
		Layout::Insets{})));
}

TEST(GuiLayoutValidation, RejectsFixedExtentsThatAreNotFiniteAndPositive) {
	for (const double extent : {0.0, -1.0, not_a_number, infinity, -infinity}) {
		EXPECT_THROW(Layout::validate(stack_with({fresh_leaf_child(Layout::Fixed{extent})})), std::invalid_argument)
			<< "extent " << extent;
	}
}

TEST(GuiLayoutValidation, RejectsFillWeightsThatAreNotFiniteAndNonNegative) {
	for (const double weight : {-1.0, not_a_number, infinity, -infinity}) {
		EXPECT_THROW(Layout::validate(stack_with({fresh_leaf_child(Layout::Fill{weight})})), std::invalid_argument)
			<< "weight " << weight;
	}
}

TEST(GuiLayoutValidation, RejectsSpacingThatIsNotFiniteAndNonNegative) {
	for (const double spacing : {-1.0, not_a_number, infinity, -infinity}) {
		EXPECT_THROW(Layout::validate(stack_with({fresh_leaf_child()}, spacing)), std::invalid_argument)
			<< "spacing " << spacing;
	}
}

TEST(GuiLayoutValidation, RejectsInvalidPaddingOnEachSide) {
	for (const double bad : {-1.0, not_a_number, infinity}) {
		const Layout::Insets cases[]{{.left = bad}, {.top = bad}, {.right = bad}, {.floor = bad}};
		for (const auto &padding : cases) {
			EXPECT_THROW(Layout::validate(stack_with({fresh_leaf_child()}, 0.0, padding)), std::invalid_argument)
				<< "padding value " << bad;
		}
	}
}

TEST(GuiLayoutValidation, RejectsAChildWithoutANode) {
	EXPECT_THROW(Layout::validate(stack_with({Layout::Child{Layout::Fixed{10}, nullptr}})), std::invalid_argument);
}

TEST(GuiLayoutValidation, RejectsTheSameComponentInTwoLeavesEvenInDifferentBranches) {
	const auto shared_id   = gen_id(ComponentType::RECTANGLE);
	const auto first_leaf  = std::make_shared<const Layout::Node>(Layout::Node{Layout::Leaf{shared_id, {}}});
	const auto second_leaf = std::make_shared<const Layout::Node>(Layout::Node{Layout::Leaf{shared_id, {}}});
	const auto nested      = std::make_shared<const Layout::Node>(stack_with({{Layout::Fixed{10}, second_leaf}}));

	EXPECT_THROW(Layout::validate(stack_with({{Layout::Fixed{10}, first_leaf}, {Layout::Fill{1.0}, nested}})),
				 std::invalid_argument);
}

TEST(GuiLayoutValidation, ReportsAnInvalidDescendantNotJustAnInvalidRoot) {
	const auto invalid_branch = std::make_shared<const Layout::Node>(stack_with({fresh_leaf_child(Layout::Fixed{-5})}));

	EXPECT_THROW(Layout::validate(stack_with({{Layout::Fill{1.0}, invalid_branch}})), std::invalid_argument);
}
