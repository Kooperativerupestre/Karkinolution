#include <gtest/gtest.h>
#include <karkinolution/math/stats/compile_values.hpp>
#include <karkinolution/math/stats/runtime_values.hpp>

// ==================== Factor ====================

TEST(Factor, AdditionClampsToMaximum) {
	constexpr float max = 2.0f;
	constexpr float min = -1.0f;

	GenericLimitedValue<float, max, min> factor;

	factor += max * 2;

	EXPECT_FLOAT_EQ(factor.value(), max);
}

TEST(Factor, AdditionClampsToMinimum) {
	constexpr float max = 2.0f;
	constexpr float min = -1.0f;

	GenericLimitedValue<float, max, min> factor;

	factor += min * 2;

	EXPECT_FLOAT_EQ(factor.value(), min);
}

TEST(Factor, SubtractionClampsToMaximum) {
	constexpr float max = 2.0f;
	constexpr float min = -1.0f;

	GenericLimitedValue<float, max, min> factor;

	factor -= min * 2;

	EXPECT_FLOAT_EQ(factor.value(), max);
}

TEST(Factor, SubtractionClampsToMinimum) {
	constexpr float max = 2.0f;
	constexpr float min = -1.0f;

	GenericLimitedValue<float, max, min> factor;

	factor -= max * 2;

	EXPECT_FLOAT_EQ(factor.value(), min);
}

TEST(Factor, MultiplicationClampsToMaximum) {
	constexpr float max = 2.0f;
	constexpr float min = -1.0f;

	GenericLimitedValue<float, max, min> factor;

	factor += 1.0f;
	factor *= max * 2;

	EXPECT_FLOAT_EQ(factor.value(), max);
}

TEST(Factor, MultiplicationClampsToMinimum) {
	constexpr float max = 2.0f;
	constexpr float min = -1.0f;

	GenericLimitedValue<float, max, min> factor;

	factor += 1.0f;
	factor *= min * 2;

	EXPECT_FLOAT_EQ(factor.value(), min);
}

TEST(Factor, DivisionClampsToMaximum) {
	constexpr float max = 2.0f;
	constexpr float min = -1.0f;

	GenericLimitedValue<float, max, min> factor;

	factor += 1.0f;
	factor /= (1 / (max * 2.0f));

	EXPECT_FLOAT_EQ(factor.value(), max);
}

TEST(Factor, DivisionClampsToMinimum) {
	constexpr float max = 2.0f;
	constexpr float min = -1.0f;

	GenericLimitedValue<float, max, min> factor;

	factor += 1.0f;
	factor /= -max / 2;

	EXPECT_FLOAT_EQ(factor.value(), min);
}

// ==================== RuntimeFactor ====================
TEST(RuntimeFactor, AdditionClampsToMaximum) {
	constexpr float max = 2.0f;
	constexpr float min = -1.0f;

	GenericRuntimeValue<float> factor{0.0f, max, min};

	factor += max * 2;

	EXPECT_FLOAT_EQ(factor.value(), max);
}

TEST(RuntimeFactor, AdditionClampsToMinimum) {
	constexpr float max = 2.0f;
	constexpr float min = -1.0f;

	GenericRuntimeValue<float> factor{0.0f, max, min};

	factor += min * 2;

	EXPECT_FLOAT_EQ(factor.value(), min);
}

TEST(RuntimeFactor, SubtractionClampsToMaximum) {
	constexpr float max = 2.0f;
	constexpr float min = -1.0f;

	GenericRuntimeValue<float> factor{0.0f, max, min};

	factor -= min * 2;

	EXPECT_FLOAT_EQ(factor.value(), max);
}

TEST(RuntimeFactor, SubtractionClampsToMinimum) {
	constexpr float max = 2.0f;
	constexpr float min = -1.0f;

	GenericRuntimeValue<float> factor{0.0f, max, min};

	factor -= max * 2;

	EXPECT_FLOAT_EQ(factor.value(), min);
}

TEST(RuntimeFactor, MultiplicationClampsToMaximum) {
	constexpr float max = 2.0f;
	constexpr float min = -1.0f;

	GenericRuntimeValue<float> factor{0.0f, max, min};

	factor += 1.0f;
	factor *= max * 2;

	EXPECT_FLOAT_EQ(factor.value(), max);
}

TEST(RuntimeFactor, MultiplicationClampsToMinimum) {
	constexpr float max = 2.0f;
	constexpr float min = -1.0f;

	GenericRuntimeValue<float> factor{0.0f, max, min};

	factor += 1.0f;
	factor *= min * 2;

	EXPECT_FLOAT_EQ(factor.value(), min);
}

TEST(RuntimeFactor, DivisionClampsToMaximum) {
	constexpr float max = 2.0f;
	constexpr float min = -1.0f;

	GenericRuntimeValue<float> factor{0.0f, max, min};

	factor += 1.0f;
	factor /= (1 / (max * 2.0f));

	EXPECT_FLOAT_EQ(factor.value(), max);
}

TEST(RuntimeFactor, DivisionClampsToMinimum) {
	constexpr float max = 2.0f;
	constexpr float min = -1.0f;

	GenericRuntimeValue<float> factor{0.0f, max, min};

	factor += 1.0f;
	factor /= -max / 2;

	EXPECT_FLOAT_EQ(factor.value(), min);
}