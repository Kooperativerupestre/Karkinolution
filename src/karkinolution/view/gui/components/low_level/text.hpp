#pragma once
#include "karkinolution/view/gui/components/base_component.hpp"
#include "karkinolution/view/gui/components/identity.hpp"
#include "karkinolution/view/gui/geometry/geometry.hpp"

#include <karkinolution/core/color.hpp>
#include <string>

namespace KarkinolutionView {
	namespace Gui {
		namespace Components {
			namespace LowLevel {
				class Text : public BaseComponent<ComponentType::TEXT> {
					private:

						// The stable maximum bound supports spatial indexing. The measured current
						// bound is centered inside it for rendering and hit-testing.
						const Geometry::Rectangle max_bound_;
						Geometry::Rectangle       current_bound_;
						std::string               text_;

					public:

						const Karkinolution::Colors::Color color;
						const GreaterThanZeroValue<int>    font_size;

						Text(const Geometry::Rectangle          &max_bound,
							 std::string                         text,
							 const Karkinolution::Colors::Color &color,
							 GreaterThanZeroValue<int>           font_size);

						[[nodiscard]]
						Geometry::Rectangle get_bounds() const {
							return max_bound_;
						}

						[[nodiscard]]
						Geometry::Rectangle get_current_bounds() const {
							return current_bound_;
						}

						[[nodiscard]]
						const std::string &get_text() const noexcept {
							return text_;
						}

						void set_text(std::string text);
						void draw() const;
				};
			} // namespace LowLevel
		} // namespace Components
	} // namespace Gui
} // namespace KarkinolutionView