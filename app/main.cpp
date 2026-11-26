#include "app/creature_board.hpp"
#include "app/synchronizer.hpp"
#include "app/window.hpp"
#include "opsiamethystos/graphics/panel/components/board.hpp"
#include "opsiamethystos/graphics/panel/components/component.hpp"
#include "opsiamethystos/graphics/panel/components/low_level/button.hpp"
#include "opsiamethystos/graphics/panel/panel.hpp"
#include "opsiamethystos/math/geometry/geometry.hpp"
#include "shared/core/color.hpp"

#include <raylib.h>
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

	KarkinolutionView::Window::init_before_loop(screen_preset);

	Panel::Panel panel{
		MathGeometry::Rectangle{MathGeometry::Position{400, 225}, MathGeometry::Width{800}, MathGeometry::Height{450}}};

	KApp::Synchronizer synchronizer{panel};

	auto button_component =
		std::make_shared<Components::Component>(Components::AllComponents{Components::LowLevel::Button::RectangleButton{
			MathGeometry::Rectangle{MathGeometry::Position{-280, -170},
									MathGeometry::Width{160},
									MathGeometry::Height{40}},
			[&synchronizer] {
				if (synchronizer.is_connected()) {
					synchronizer.disconnect();
				} else {
					synchronizer.connect();
				}
			},
			Components::LowLevel::Button::ButtonStyles{Colors::Presets::DarkGray,
													   Colors::Presets::Gray,
													   Colors::Presets::Black},
			"Connect",
			Colors::Presets::White,
			20}});
	if (!panel.add(button_component)) {
		throw std::runtime_error("Failed to add connection button to its panel");
	}

	std::string connection_button_label{"Connect"};


	Components::Board board{
		MathGeometry::Rectangle{MathGeometry::Position{0, 0}, MathGeometry::Width{400}, MathGeometry::Height{240}}};
	KarkinolutionView::CreatureBoard creature_board{board};

	while (!WindowShouldClose()) {

		KarkinolutionView::Window::init_inside_loop();


		panel.draw();

		KarkinolutionView::Window::end_inside_loop();
	}
	KarkinolutionView::Window::end_after_loop();

	return 0;
}
