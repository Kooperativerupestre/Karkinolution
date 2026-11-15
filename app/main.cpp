#include "app/screen.hpp"
#include "app/synchronizer.hpp"
#include "app/window.hpp"
#include "karkinolution/networking/client_side/client/alias.hpp"
#include "opsiamethystos/gui/components/board.hpp"
#include "opsiamethystos/gui/components/component.hpp"
#include "opsiamethystos/gui/components/low_level/button.hpp"
#include "opsiamethystos/gui/panel/panel.hpp"
#include "opsiamethystos/math/geometry/geometry.hpp"
#include "opsiamethystos/raylib_utils/colors.hpp"
#include "shared/core/color.hpp"

#include <app/presets/aliases.hpp>
#include <opsiamethystos/raylib_utils/colors.hpp>
#include <raylib.h>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>

namespace GuiGeometry = Opsiamethystos::Gui::Geometry;
namespace Colors      = KShared::Colors;
namespace Components  = Opsiamethystos::Gui::Components;
namespace Panel       = Opsiamethystos::Gui::Panel;
using KarkinolutionView::Synchronizer;

int main() {
	KPresets::Screen screen_preset{.width = 800, .height = 450};

	KarkinolutionView::Window::init_before_loop(screen_preset);

	Panel::Panel panel{
		GuiGeometry::Rectangle{GuiGeometry::Position{400, 225}, GuiGeometry::Width{800}, GuiGeometry::Height{450}}};

	Synchronizer synchronizer{panel};

	auto button_component =
		std::make_shared<Components::Component>(Components::AllComponents{Components::LowLevel::Button::RectangleButton{
			GuiGeometry::Rectangle{GuiGeometry::Position{-280, -170}, GuiGeometry::Width{160}, GuiGeometry::Height{40}},
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
		GuiGeometry::Rectangle{GuiGeometry::Position{0, 35}, GuiGeometry::Width{400}, GuiGeometry::Height{240}}};
	if (!board.draw_rectangle(board.get_bounds(), Colors::Presets::Black)
		|| !board.draw_circle(GuiGeometry::Circle{{-120, 35}, GuiGeometry::Radius{35}}, Colors::Presets::Red)
		|| !board.draw_circle(GuiGeometry::Circle{{0, 35}, GuiGeometry::Radius{35}}, Colors::Presets::Green)
		|| !board.draw_circle(GuiGeometry::Circle{{120, 35}, GuiGeometry::Radius{35}}, Colors::Presets::Blue)) {
		throw std::runtime_error("Failed to draw demo primitive on board");
	}

	while (!WindowShouldClose()) {
		BeginDrawing();

		KarkinolutionView::Window::init_inside_loop();

		synchronizer.draw();
		board.draw(panel.origin());

		const std::string_view connection_label = synchronizer.is_connected() ? "Disconnect" : "Connect";
		if (connection_label != connection_button_label) {
			std::get<Components::LowLevel::Button::RectangleButton>(button_component->get_data())
				.set_text(std::string{connection_label});
			connection_button_label = std::string{connection_label};
		}

		for (const auto id : panel.collides(GuiGeometry::Rectangle{GuiGeometry::Position{0, 0},
																   GuiGeometry::Width{800},
																   GuiGeometry::Height{450}})) {
			const auto component = panel.get(id);
			if (!component) {
				throw std::logic_error("Panel collision returned an unknown component");
			}

			std::visit(
				[&panel](auto &visible_component) {
					if constexpr (std::is_same_v<std::remove_cvref_t<decltype(visible_component)>,
												 Components::LowLevel::Button::RectangleButton>) {
						visible_component.run(panel.origin());
					} else if constexpr (requires { visible_component.draw(panel.origin()); }) {
						visible_component.draw(panel.origin());
					}
				},
				component->get_data());
		}

		KarkinolutionView::Window::end_inside_loop();
	}
	KarkinolutionView::Window::end_after_loop();

	return 0;
}
