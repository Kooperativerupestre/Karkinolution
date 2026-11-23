#include <gtest/gtest.h>
#include <shared/math/stats/compile_values.hpp>
#include <shared/math/stats/runtime_values.hpp>

using KShared::Math::LV::CT::GenericLimitedValue;
using KShared::Math::LV::RT::GenericRuntimeValue;

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

TEST(GreaterThanZeroValue, IntegralMinIsOne) {
	using KShared::Math::LV::CT::GreaterThanZeroValue;

	GreaterThanZeroValue<int> v{5};
	EXPECT_EQ(v.value(), 5);

	GreaterThanZeroValue<int> clamped{0};
	EXPECT_EQ(clamped.value(), 1);

	GreaterThanZeroValue<int> negative{-10};
	EXPECT_EQ(negative.value(), 1);
}

TEST(GreaterThanZeroValue, FloatMinIsEpsilonBased) {
	using KShared::Math::LV::CT::GreaterThanZeroValue;

	GreaterThanZeroValue<float> v{1.0f};
	EXPECT_GT(v.value(), 0.0f);

	GreaterThanZeroValue<float> clamped{0.0f};
	EXPECT_GT(clamped.value(), 0.0f);
}

TEST(LimitedValueRatio, NormalRangeRatioIsCorrect) {
	using KShared::Math::LV::CT::GenericLimitedValue;

	constexpr float max = 10.0f;
	constexpr float min = 0.0f;

	GenericLimitedValue<float, max, min> v{5.0f};
	EXPECT_FLOAT_EQ(v.ratio().value(), 0.5f);

	GenericLimitedValue<float, max, min> at_min{0.0f};
	EXPECT_FLOAT_EQ(at_min.ratio().value(), 0.0f);

	GenericLimitedValue<float, max, min> at_max{10.0f};
	EXPECT_FLOAT_EQ(at_max.ratio().value(), 1.0f);
}

TEST(Factor, DivisionByZeroThrows) {
	constexpr float max = 2.0f;
	constexpr float min = -1.0f;

	GenericLimitedValue<float, max, min> factor{1.0f};

	EXPECT_THROW(factor /= 0.0f, std::invalid_argument);
	EXPECT_THROW(factor /= 0, std::invalid_argument);
	EXPECT_THROW([[maybe_unused]] auto res = factor / 0.0f, std::invalid_argument);
	EXPECT_THROW([[maybe_unused]] auto res = factor / 0, std::invalid_argument);
}

TEST(RuntimeFactor, DivisionByZeroThrows) {
	constexpr float max = 2.0f;
	constexpr float min = -1.0f;

	GenericRuntimeValue<float> factor{1.0f, max, min};

	EXPECT_THROW(factor /= 0.0f, std::invalid_argument);
	EXPECT_THROW(factor /= 0, std::invalid_argument);
}

TEST(LimitedValueInteger, DivisionByZeroThrows) {
	GenericLimitedValue<int, 100, 0> val{50};

	EXPECT_THROW(val /= 0, std::invalid_argument);
	EXPECT_THROW([[maybe_unused]] auto res = val / 0, std::invalid_argument);
}
