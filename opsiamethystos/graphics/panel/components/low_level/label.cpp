#include "opsiamethystos/graphics/panel/components/low_level/label.hpp"

#include <utility>

namespace Opsiamethystos::Graphics::Panel::Components {

	namespace {
		LowLevel::Rectangle make_rectangle(const Math::Geometry::Rectangle &bounds,
										   const KShared::Colors::Color    &color) {
			return LowLevel::Rectangle{bounds, color};
		}
	} // namespace

	LowLevel::RectangleLabel::RectangleLabel(const Math::Geometry::Rectangle &bounds,
											 std::string                      text,
											 const KShared::Colors::Color    &rectangle_color,
											 const KShared::Colors::Color    &text_color,
											 GreaterThanZeroValue<int>        font_size)
		: rectangle_(make_rectangle(bounds, rectangle_color))
		, text_(bounds, std::move(text), text_color, font_size) {}

	void LowLevel::RectangleLabel::set_text(std::string text) {
		text_.set_text(std::move(text));
	}

	void LowLevel::RectangleLabel::draw() const {
		draw({0, 0});
	}

	void LowLevel::RectangleLabel::draw(const Math::Geometry::Position &origin) const {
		rectangle_.draw(origin);
		text_.draw(origin);
	}

} // namespace Opsiamethystos::Graphics::Panel::Components
