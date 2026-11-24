#include "app/creature_board.hpp"
#include "app/synchronizer.hpp"
#include "app/window.hpp"
#include "opsiamethystos/graphics/panel/components/board.hpp"
#include "opsiamethystos/graphics/panel/components/component.hpp"
#include "opsiamethystos/graphics/panel/components/low_level/button.hpp"
#include "opsiamethystos/graphics/panel/panel.hpp"
#include "opsiamethystos/math/geometry/geometry.hpp"
#include "shared/core/color.hpp"

#include <app/presets/aliases.hpp>
#include <raylib.h>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>

namespace MathGeometry = Opsiamethystos::Math::Geometry;
namespace Colors       = KShared::Colors;
namespace Components   = Opsiamethystos::Gui::Components;
namespace Panel        = Opsiamethystos::Gui::Panel;
using KarkinolutionView::Synchronizer;

int main() {
	KPresets::Screen screen_preset{.width = 800, .height = 450};

	KarkinolutionView::Window::init_before_loop(screen_preset);

	Panel::Panel panel{
		MathGeometry::Rectangle{MathGeometry::Position{400, 225}, MathGeometry::Width{800}, MathGeometry::Height{450}}};

	Synchronizer synchronizer{panel};

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

	auto create_button_component =
		std::make_shared<Components::Component>(Components::AllComponents{Components::LowLevel::Button::RectangleButton{
			MathGeometry::Rectangle{MathGeometry::Position{-90, -170},
									MathGeometry::Width{160},
									MathGeometry::Height{40}},
			[&synchronizer] {
				synchronizer.create_creature();
			},
			Components::LowLevel::Button::ButtonStyles{Colors::Presets::DarkGray,
													   Colors::Presets::Gray,
													   Colors::Presets::Black},
			"Create creature",
			Colors::Presets::White,
			20}});
	if (!panel.add(create_button_component)) {
		throw std::runtime_error("Failed to add creature creation button to its panel");
	}

	Components::Board board{
		MathGeometry::Rectangle{MathGeometry::Position{0, 0}, MathGeometry::Width{400}, MathGeometry::Height{240}}};
	KarkinolutionView::CreatureBoard creature_board{board};

	while (!WindowShouldClose()) {
		BeginDrawing();

		KarkinolutionView::Window::init_inside_loop();

		synchronizer.poll();
		synchronizer.draw();
		creature_board.draw(synchronizer.creatures(), synchronizer.territory_size());
		board.draw(panel.origin());

		const std::string_view connection_label = synchronizer.is_connected() ? "Disconnect" : "Connect";
		if (connection_label != connection_button_label) {
			std::get<Components::LowLevel::Button::RectangleButton>(button_component->get_data())
				.set_text(std::string{connection_label});
			connection_button_label = std::string{connection_label};
		}

		for (const auto id : panel.collides(MathGeometry::Rectangle{MathGeometry::Position{0, 0},
																	MathGeometry::Width{800},
																	MathGeometry::Height{450}})) {
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
