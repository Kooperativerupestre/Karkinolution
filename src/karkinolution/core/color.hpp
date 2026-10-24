#pragma once

#include "karkinolution/math/stats/compile_values.hpp"

namespace Karkinolution {
	namespace Colors {

		namespace Components {
			struct Red {
					NormalizedValue<float> value;

					constexpr bool operator==(const Red &) const noexcept = default;
			};

			struct Green {
					NormalizedValue<float> value;

					constexpr bool operator==(const Green &) const noexcept = default;
			};

			struct Blue {
					NormalizedValue<float> value;

					constexpr bool operator==(const Blue &) const noexcept = default;
			};

			struct Alpha {
					NormalizedValue<float> value;

					constexpr bool operator==(const Alpha &) const noexcept = default;
			};
		} // namespace Components

		class Color {
			public:

				Components::Red   r;
				Components::Green g;
				Components::Blue  b;
				Components::Alpha a;

				constexpr bool operator==(const Color &) const noexcept = default;

				// Multiplication by a normalized scalar.
				constexpr Color operator*(NormalizedValue<float> factor) const noexcept {
					return {.r = Components::Red{r.value * factor},
							.g = Components::Green{g.value * factor},
							.b = Components::Blue{b.value * factor},
							.a = Components::Alpha{a.value * factor}};
				}

				// Division by a normalized scalar.
				constexpr Color operator/(NormalizedValue<float> factor) const {
					return {.r = Components::Red{r.value / factor},
							.g = Components::Green{g.value / factor},
							.b = Components::Blue{b.value / factor},
							.a = Components::Alpha{a.value / factor}};
				}

				constexpr Color &operator*=(NormalizedValue<float> factor) noexcept {
					r.value *= factor;
					g.value *= factor;
					b.value *= factor;
					a.value *= factor;
					return *this;
				}

				constexpr Color &operator/=(NormalizedValue<float> factor) {
					r.value /= factor;
					g.value /= factor;
					b.value /= factor;
					a.value /= factor;
					return *this;
				}

				// Returns the same color with a different alpha component.
				constexpr Color with_alpha(Components::Alpha alpha) const noexcept {
					return {.r = r, .g = g, .b = b, .a = alpha};
				}

				// Multiplies RGB without changing alpha.
				constexpr Color
				multiply_without_alpha(NormalizedValue<float> factor) const noexcept {
					return multiply(factor, 1.0f);
				}

				constexpr Color multiply(NormalizedValue<float> rgb_factor,
										 NormalizedValue<float> alpha_factor) const noexcept {
					return Color{.r = Components::Red{r.value * rgb_factor},
								 .g = Components::Green{g.value * rgb_factor},
								 .b = Components::Blue{b.value * rgb_factor},
								 .a = Components::Alpha{a.value * alpha_factor}};
				}

				constexpr Color divide(NormalizedValue<float> rgb_factor,
									   NormalizedValue<float> alpha_factor) const noexcept {
					return Color{.r = Components::Red{r.value / rgb_factor},
								 .g = Components::Green{g.value / rgb_factor},
								 .b = Components::Blue{b.value / rgb_factor},
								 .a = Components::Alpha{a.value / alpha_factor}};
				}

				// Moves RGB components toward white.
				constexpr Color lighten(NormalizedValue<float> factor) const noexcept {
					return {
						.r = Components::Red{r.value + (1.0f - r.value.value()) * factor.value()},
						.g = Components::Green{g.value + (1.0f - g.value.value()) * factor.value()},
						.b = Components::Blue{b.value + (1.0f - b.value.value()) * factor.value()},
						.a = a};
				}

				// Inverts RGB without changing alpha.
				constexpr Color invert() const noexcept {
					return {.r = Components::Red{1.0f - r.value.value()},
							.g = Components::Green{1.0f - g.value.value()},
							.b = Components::Blue{1.0f - b.value.value()},
							.a = a};
				}

				// Linear interpolation between two colors.
				static constexpr Color
				lerp(const Color &from, const Color &to, NormalizedValue<float> t) noexcept {


					return {.r = Components::Red{from.r.value + (to.r.value - from.r.value) * t},
							.g = Components::Green{from.g.value + (to.g.value - from.g.value) * t},
							.b = Components::Blue{from.b.value + (to.b.value - from.b.value) * t},
							.a = Components::Alpha{from.a.value + (to.a.value - from.a.value) * t}};
				}

				// gradient functions don't include the color that called it
				template <size_t N>
				constexpr std::array<Color, N>
				gradient_strong_to_weak(NormalizedValue<float> rgb_factor,
										NormalizedValue<float> alpha_factor) {
					std::array<Color, N> gradient;

					for (size_t i = 0; i < N; i++) {
						gradient[i] = Color{
							// i + 1 -> The "+1" satisfies the criterium that the first element
							// of the array will not be equal the original color
							.r = Components::Red{this->r.value - rgb_factor.value() * (i + 1)},
							.g = Components::Green{this->g.value - rgb_factor.value() * (i + 1)},
							.b = Components::Blue{this->b.value - rgb_factor.value() * (i + 1)},
							.a = Components::Alpha{this->a.value - alpha_factor.value() * (i + 1)}};
					}
					return gradient;
				}

				template <size_t N>
				constexpr std::array<Color, N>
				gradient_weak_to_strong(NormalizedValue<float> rgb_factor,
										NormalizedValue<float> alpha_factor) {
					std::array<Color, N> gradient;

					for (size_t i = 0; i < N; i++) {
						gradient[i] = Color{
							// N - i + 1 -> The "+1" satisfies the criterium that the first element
							// of the array will not be equal the original color
							.r = Components::Red{this->r.value - rgb_factor.value() * (N - i + 1)},
							.g =
								Components::Green{this->g.value - rgb_factor.value() * (N - i + 1)},
							.b = Components::Blue{this->b.value - rgb_factor.value() * (N - i + 1)},
							.a = Components::Alpha{this->a.value
												   - alpha_factor.value() * (N - i + 1)}};
					}
					return gradient;
				}
		};

		// Allows scalar * color as well as color * scalar.
		constexpr Color operator*(NormalizedValue<float> factor, const Color &color) noexcept {
			return color * factor;
		}

		namespace Presets {


			// Basic colors
			inline constexpr Color Black{.r = Components::Red{0.0f},
										 .g = Components::Green{0.0f},
										 .b = Components::Blue{0.0f},
										 .a = Components::Alpha{1.0f}};

			inline constexpr Color White{.r = Components::Red{1.0f},
										 .g = Components::Green{1.0f},
										 .b = Components::Blue{1.0f},
										 .a = Components::Alpha{1.0f}};

			inline constexpr Color Red{.r = Components::Red{1.0f},
									   .g = Components::Green{0.0f},
									   .b = Components::Blue{0.0f},
									   .a = Components::Alpha{1.0f}};

			inline constexpr Color Green{.r = Components::Red{0.0f},
										 .g = Components::Green{1.0f},
										 .b = Components::Blue{0.0f},
										 .a = Components::Alpha{1.0f}};

			inline constexpr Color Blue{.r = Components::Red{0.0f},
										.g = Components::Green{0.0f},
										.b = Components::Blue{1.0f},
										.a = Components::Alpha{1.0f}};

			inline constexpr Color Yellow{.r = Components::Red{1.0f},
										  .g = Components::Green{1.0f},
										  .b = Components::Blue{0.0f},
										  .a = Components::Alpha{1.0f}};

			inline constexpr Color Cyan{.r = Components::Red{0.0f},
										.g = Components::Green{1.0f},
										.b = Components::Blue{1.0f},
										.a = Components::Alpha{1.0f}};

			inline constexpr Color Magenta{.r = Components::Red{1.0f},
										   .g = Components::Green{0.0f},
										   .b = Components::Blue{1.0f},
										   .a = Components::Alpha{1.0f}};

			// Warm colors
			inline constexpr Color Orange{.r = Components::Red{1.0f},
										  .g = Components::Green{0.647f},
										  .b = Components::Blue{0.0f},
										  .a = Components::Alpha{1.0f}};

			inline constexpr Color Gold{.r = Components::Red{1.0f},
										.g = Components::Green{0.843f},
										.b = Components::Blue{0.0f},
										.a = Components::Alpha{1.0f}};

			inline constexpr Color DarkRed{.r = Components::Red{0.545f},
										   .g = Components::Green{0.0f},
										   .b = Components::Blue{0.0f},
										   .a = Components::Alpha{1.0f}};

			inline constexpr Color Crimson{.r = Components::Red{0.863f},
										   .g = Components::Green{0.078f},
										   .b = Components::Blue{0.235f},
										   .a = Components::Alpha{1.0f}};

			inline constexpr Color Pink{.r = Components::Red{1.0f},
										.g = Components::Green{0.753f},
										.b = Components::Blue{0.796f},
										.a = Components::Alpha{1.0f}};

			// Cool colors
			inline constexpr Color SkyBlue{.r = Components::Red{0.529f},
										   .g = Components::Green{0.808f},
										   .b = Components::Blue{0.922f},
										   .a = Components::Alpha{1.0f}};

			inline constexpr Color Navy{.r = Components::Red{0.0f},
										.g = Components::Green{0.0f},
										.b = Components::Blue{0.502f},
										.a = Components::Alpha{1.0f}};

			inline constexpr Color Purple{.r = Components::Red{0.502f},
										  .g = Components::Green{0.0f},
										  .b = Components::Blue{0.502f},
										  .a = Components::Alpha{1.0f}};

			inline constexpr Color Violet{.r = Components::Red{0.933f},
										  .g = Components::Green{0.510f},
										  .b = Components::Blue{0.933f},
										  .a = Components::Alpha{1.0f}};

			inline constexpr Color Teal{.r = Components::Red{0.0f},
										.g = Components::Green{0.502f},
										.b = Components::Blue{0.502f},
										.a = Components::Alpha{1.0f}};

			// Earth tones
			inline constexpr Color Brown{.r = Components::Red{0.545f},
										 .g = Components::Green{0.271f},
										 .b = Components::Blue{0.075f},
										 .a = Components::Alpha{1.0f}};

			inline constexpr Color Tan{.r = Components::Red{0.824f},
									   .g = Components::Green{0.706f},
									   .b = Components::Blue{0.549f},
									   .a = Components::Alpha{1.0f}};

			inline constexpr Color Beige{.r = Components::Red{0.961f},
										 .g = Components::Green{0.961f},
										 .b = Components::Blue{0.863f},
										 .a = Components::Alpha{1.0f}};

			// Neutral colors
			inline constexpr Color Gray{.r = Components::Red{0.502f},
										.g = Components::Green{0.502f},
										.b = Components::Blue{0.502f},
										.a = Components::Alpha{1.0f}};

			inline constexpr Color Silver{.r = Components::Red{0.753f},
										  .g = Components::Green{0.753f},
										  .b = Components::Blue{0.753f},
										  .a = Components::Alpha{1.0f}};

			inline constexpr Color DarkGray{.r = Components::Red{0.251f},
											.g = Components::Green{0.251f},
											.b = Components::Blue{0.251f},
											.a = Components::Alpha{1.0f}};

			// Special colors
			inline constexpr Color Transparent{.r = Components::Red{0.0f},
											   .g = Components::Green{0.0f},
											   .b = Components::Blue{0.0f},
											   .a = Components::Alpha{0.0f}};

		} // namespace Presets

	} // namespace Colors
} // namespace Karkinolution
