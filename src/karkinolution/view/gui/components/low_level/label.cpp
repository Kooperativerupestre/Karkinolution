#include "karkinolution/view/gui/components/low_level/label.hpp"

#include <utility>

namespace KarkinolutionView {
	namespace Gui {
		namespace Components {
			namespace LowLevel {
				namespace {
					Rectangle make_rectangle(const Geometry::Rectangle          &bounds,
											 const KShared::Colors::Color &color) {
						return Rectangle{bounds, color};
					}
				} // namespace

				RectangleLabel::RectangleLabel(const Geometry::Rectangle          &bounds,
											   std::string                         text,
											   const KShared::Colors::Color &rectangle_color,
											   const KShared::Colors::Color &text_color,
											   GreaterThanZeroValue<int>           font_size)
					: rectangle_(make_rectangle(bounds, rectangle_color))
					, text_(bounds, std::move(text), text_color, font_size) {}

				void RectangleLabel::set_text(std::string text) {
					text_.set_text(std::move(text));
				}

				void RectangleLabel::draw() const {
					draw({0, 0});
				}

				void RectangleLabel::draw(const Geometry::Position &origin) const {
					rectangle_.draw(origin);
					text_.draw(origin);
				}
			} // namespace LowLevel
		} // namespace Components
	} // namespace Gui
} // namespace KarkinolutionView
