#include "app/aliases.hpp"
#include "app/logger.hpp"
#include "app/synchronizer.hpp"
#include "app/window.hpp"
#include "opsiamethystos/graphics/panel/components/board.hpp"
#include "opsiamethystos/graphics/panel/components/component.hpp"
#include "opsiamethystos/graphics/panel/components/low_level/button.hpp"
#include "opsiamethystos/graphics/panel/layout_error.hpp"
#include "opsiamethystos/graphics/panel/panel.hpp"
#include "opsiamethystos/math/geometry/geometry.hpp"
#include "shared/core/color.hpp"

#include <format>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>

using namespace KApp;

int main() {
	KApp::app_logger.log<LogLevel::INFO>("App is initializing");

	KApp::Presets::Screen screen_preset{.width = 800, .height = 450};

	KApp::app_logger.log<LogLevel::INFO>(
		std::format("The width of screen is {} and the height is {}", screen_preset.width, screen_preset.height));

	KApp::Window::init_before_loop(screen_preset);

	Panel panel{Opsiamethystos::Math::Geometry::Rectangle{Opsiamethystos::Math::Geometry::Position{400, 225},
														  Opsiamethystos::Math::Geometry::Width{800},
														  Opsiamethystos::Math::Geometry::Height{450}}};
	KApp::Synchronizer synchronizer{panel};


	app_logger.log<LogLevel::DEBUG>("The windows is initializing");
	while (!KApp::Window::should_window_close()) {

		KApp::Window::init_inside_loop();


		const auto layout = panel.update();
		if (!layout) {
			throw std::runtime_error(std::string{Opsiamethystos::Gui::Panel::to_string(layout.error().code)});
		}
		assert(!layout->overflowed);

		panel.draw();


		KApp::Window::end_inside_loop();
	}
	KApp::Window::end_after_loop();

	return 0;
}
