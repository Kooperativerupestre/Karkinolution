#include <gtest/gtest.h>
#include <shared/math/physic/vec/model.hpp>
#include <type_traits>

TEST(Vec2Test, CompoundAssignmentReturnReference) {
	using KShared::Math::Vec::Vec2;

	Vec2 a(1.0, 2.0);
	Vec2 b(3.0, 4.0);

	static_assert(std::is_same_v<decltype(a += b), Vec2 &>);
	static_assert(std::is_same_v<decltype(a -= b), Vec2 &>);
	static_assert(std::is_same_v<decltype(a *= 2.0), Vec2 &>);
	static_assert(std::is_same_v<decltype(a /= 2.0), Vec2 &>);

	Vec2 &ref_add = (a += b);
	EXPECT_EQ(&ref_add, &a);
	EXPECT_DOUBLE_EQ(a.x, 4.0);
	EXPECT_DOUBLE_EQ(a.y, 6.0);

	// Chaining
	(a += b) += b;
	EXPECT_DOUBLE_EQ(a.x, 10.0);
	EXPECT_DOUBLE_EQ(a.y, 14.0);

	Vec2 &ref_sub = (a -= b);
	EXPECT_EQ(&ref_sub, &a);
	EXPECT_DOUBLE_EQ(a.x, 7.0);
	EXPECT_DOUBLE_EQ(a.y, 10.0);

	(a -= b) -= b;
	EXPECT_DOUBLE_EQ(a.x, 1.0);
	EXPECT_DOUBLE_EQ(a.y, 2.0);

	Vec2 &ref_mul = (a *= 3.0);
	EXPECT_EQ(&ref_mul, &a);
	EXPECT_DOUBLE_EQ(a.x, 3.0);
	EXPECT_DOUBLE_EQ(a.y, 6.0);

	(a *= 2.0) *= 2.0;
	EXPECT_DOUBLE_EQ(a.x, 12.0);
	EXPECT_DOUBLE_EQ(a.y, 24.0);

	Vec2 &ref_div = (a /= 2.0);
	EXPECT_EQ(&ref_div, &a);
	EXPECT_DOUBLE_EQ(a.x, 6.0);
	EXPECT_DOUBLE_EQ(a.y, 12.0);

	(a /= 2.0) /= 3.0;
	EXPECT_DOUBLE_EQ(a.x, 1.0);
	EXPECT_DOUBLE_EQ(a.y, 2.0);
}
