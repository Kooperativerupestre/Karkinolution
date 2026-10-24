#include <gtest/gtest.h>
#include <karkinolution/core/color.hpp>

TEST(Color, GradientsFirstElementIsNotEqualOriginalColor) {
	auto color = Karkinolution::Colors::Presets::Red;

	const auto gradient_strong_to_weak = color.gradient_strong_to_weak<10>(0.05f, 0.0f);
	const auto gradient_weak_to_strong = color.gradient_weak_to_strong<10>(0.05f, 0.0f);

	EXPECT_FALSE(color == gradient_strong_to_weak[0]);
	EXPECT_FALSE(color == gradient_weak_to_strong[0]);
}

TEST(Color, GradientStrongToWeakOrder) {
	auto color = Karkinolution::Colors::Presets::SkyBlue;

	const auto gradient = color.gradient_strong_to_weak<10>(0.05f, 0.053f);

	for (size_t i = 0; i < gradient.size(); i++) {
		if (i == gradient.size() - 1) {
			break;
		}
		EXPECT_TRUE(gradient[0].r.value > gradient[1].r.value);
		EXPECT_TRUE(gradient[0].g.value > gradient[1].g.value);
		EXPECT_TRUE(gradient[0].b.value > gradient[1].b.value);
		EXPECT_TRUE(gradient[0].a.value > gradient[1].a.value);
	}
}

TEST(Color, GradientWeakToStrongOrder) {
	auto color = Karkinolution::Colors::Presets::SkyBlue;

	const auto gradient = color.gradient_weak_to_strong<10>(0.05f, 0.053f);

	for (size_t i = 0; i < gradient.size(); i++) {
		if (i == gradient.size() - 1) {
			break;
		}
		EXPECT_TRUE(gradient[0].r.value < gradient[1].r.value);
		EXPECT_TRUE(gradient[0].g.value < gradient[1].g.value);
		EXPECT_TRUE(gradient[0].b.value < gradient[1].b.value);
		EXPECT_TRUE(gradient[0].a.value < gradient[1].a.value);
	}
}
