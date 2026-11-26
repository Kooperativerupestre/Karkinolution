#include "app/synchronizer.hpp"
#include "app/window.hpp"
#include "opsiamethystos/graphics/panel/components/board.hpp"
#include "opsiamethystos/graphics/panel/components/component.hpp"
#include "opsiamethystos/graphics/panel/components/low_level/button.hpp"
#include "opsiamethystos/graphics/panel/panel.hpp"
#include "opsiamethystos/math/geometry/geometry.hpp"
#include "shared/core/color.hpp"

#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>

namespace MathGeometry = Opsiamethystos::Math::Geometry;
namespace Colors       = KShared::Colors;
namespace Components   = Opsiamethystos::Gui::Components;
namespace Panel        = Opsiamethystos::Gui::Panel;

int main() {
	KApp::Presets::Screen screen_preset{.width = 800, .height = 450};

	KApp::Window::init_before_loop(screen_preset);

	Panel::Panel panel{
		MathGeometry::Rectangle{MathGeometry::Position{400, 225}, MathGeometry::Width{800}, MathGeometry::Height{450}}};
	panel.set_layout(Panel::Layout::Node{Panel::Layout::Stack{.direction   = Panel::Layout::Direction::HORIZONTAL,
															  .cross_align = Panel::Layout::CrossAlign::STRETCH,
															  .children    = {}}});
	KApp::Synchronizer synchronizer{panel};


	while (!KApp::Window::should_window_close()) {

		KApp::Window::init_inside_loop();


		panel.draw();
		const auto result = panel.relayout();

		assert(!result.overflowed);

		KApp::Window::end_inside_loop();
	}
	KApp::Window::end_after_loop();

	return 0;
}
