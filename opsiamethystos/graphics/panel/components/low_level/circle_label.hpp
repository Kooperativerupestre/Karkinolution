#pragma once

#include "opsiamethystos/graphics/panel/components/base_component.hpp"
#include "opsiamethystos/graphics/panel/components/identity.hpp"
#include "opsiamethystos/graphics/panel/components/low_level/circle.hpp"
#include "opsiamethystos/graphics/panel/components/low_level/text.hpp"
#include "opsiamethystos/math/geometry/geometry.hpp"
#include "shared/core/color.hpp"

#include <string>

namespace Opsiamethystos {
	namespace Graphics {
		namespace Panel {
			namespace Components {
				namespace LowLevel {
					class CircleLabel
						: public Gui::Components::BaseComponent<Gui::Components::ComponentType::CIRCLE_LABEL> {
						private:

							Circle circle_;
							Text   text_;

						public:

							CircleLabel(const Math::Geometry::Circle &bounds,
										std::string                   text,
										const KShared::Colors::Color &circle_color,
										const KShared::Colors::Color &text_color,
										GreaterThanZeroValue<int>     font_size);

							[[nodiscard]]
							Math::Geometry::Circle get_bounds() const {
								return circle_.get_bounds();
							}

							[[nodiscard]]
							Math::Geometry::Rectangle get_current_bounds() const {
								return text_.get_current_bounds();
							}

							[[nodiscard]]
							const std::string &get_text() const noexcept {
								return text_.get_text();
							}

							void set_text(std::string text);

							// The circle and the text are one compound update. The old circle is
							// reconstructed if the text refuses the proposed slot; Text accepts every
							// slot today, so this guards against a future change, not a current case.
							bool set_slot(const Math::Geometry::Rectangle &slot) {
								const auto old_slot = circle_.get_bounds();
								if (!circle_.set_slot(slot)) {
									return false;
								}
								if (!text_.set_slot(slot)) {
									const auto old_rectangle = Math::Geometry::Rectangle{
										old_slot.position,
										Math::Geometry::Width{static_cast<double>(old_slot.radius.value()) * 2.0},
										Math::Geometry::Height{static_cast<double>(old_slot.radius.value()) * 2.0}};
									circle_.set_slot(old_rectangle);
									return false;
								}
								return true;
							}

							void draw() const;
							// Both the fitted circle and centered text receive the same translation.
							void draw(const Math::Geometry::Position &origin) const;
					};
				} // namespace LowLevel
			} // namespace Components
		} // namespace Panel
	} // namespace Graphics
} // namespace Opsiamethystos
