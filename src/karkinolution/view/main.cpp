#include "shared/core/color.hpp"
#include "karkinolution/networking/sdk/src/client/alias.hpp"
#include "karkinolution/view/client/client.hpp"
#include "karkinolution/view/gui/colors.hpp"
#include "karkinolution/view/gui/components/component.hpp"
#include "karkinolution/view/gui/components/low_level/button.hpp"
#include "karkinolution/view/gui/components/medium_level/grid.hpp"
#include "karkinolution/view/gui/geometry/geometry.hpp"
#include "karkinolution/view/gui/panel/panel.hpp"
#include "karkinolution/view/screen.hpp"
#include "karkinolution/view/synchronizer.hpp"
#include "karkinolution/view/window.hpp"

#include <array>
#include <karkinolution/view/gui/colors.hpp>
#include <karkinolution/view/presets/aliases.hpp>
#include <karkinolution/view/presets/grid.hpp>
#include <random>
#include <raylib.h>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

namespace GuiGeometry = KarkinolutionView::Gui::Geometry;
namespace Colors      = KShared::Colors;
namespace Components  = KarkinolutionView::Gui::Components;
namespace Panel       = KarkinolutionView::Gui::Panel;
using KarkinolutionView::Synchronizer;

int main() {
	KPresets::Screen screen_preset{.width = 800, .height = 450};

	KarkinolutionView::Window::init_before_loop(screen_preset);

	Synchronizer                                  synchronizer;
	const GuiGeometry::Rectangle                  connection_button_bounds{GuiGeometry::Position{100, 40},
                                                          GuiGeometry::Width{160},
                                                          GuiGeometry::Height{40}};
	Components::LowLevel::Button::RectangleButton connection_button{
		connection_button_bounds,
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
		Components::LowLevel::Text{connection_button_bounds, "Connect", Colors::Presets::White, 20}};
	std::string  connection_button_label{"Connect"};
	Panel::Panel panel{
		GuiGeometry::Rectangle{GuiGeometry::Position{400, 225}, GuiGeometry::Width{800}, GuiGeometry::Height{450}}};
	auto grid_component = std::make_shared<Components::Component>(
		Components::AllComponents{KarkinolutionView::Gui::Components::MediumLevel::Grid{
			GuiGeometry::Rectangle{GuiGeometry::Position{0, 35}, GuiGeometry::Width{400}, GuiGeometry::Height{240}},
			GuiGeometry::Thickness{5},
			GuiGeometry::Width{50},
			GuiGeometry::Height{40},
			Colors::Presets::DarkGray}});
	if (!panel.add(grid_component)) {
		throw std::runtime_error("Failed to add the demo grid to its panel");
	}
	const auto &grid = std::get<KarkinolutionView::Gui::Components::MediumLevel::Grid>(grid_component->get_data());

	const std::array<KShared::Colors::Color, 8> cell_colors{Colors::Presets::Red,
																  Colors::Presets::Orange,
																  Colors::Presets::Yellow,
																  Colors::Presets::Green,
																  Colors::Presets::Blue,
																  Colors::Presets::Purple,
																  Colors::Presets::Pink,
																  Colors::Presets::Teal};
	std::mt19937                                      random_engine{std::random_device{}()};
	std::uniform_int_distribution<std::size_t>        random_color{0, cell_colors.size() - 1};
	std::vector<KShared::Colors::Color>         grid_colors;
	grid_colors.reserve(static_cast<std::size_t>(grid.rows().value() * grid.columns().value()));
	for (int row = 0; row < grid.rows().value(); ++row) {
		for (int column = 0; column < grid.columns().value(); ++column) {
			grid_colors.push_back(cell_colors[random_color(random_engine)]);
		}
	}

	while (!WindowShouldClose()) {
		BeginDrawing();

		KarkinolutionView::Window::init_inside_loop();

		synchronizer.draw();

		const std::string_view connection_label = synchronizer.is_connected() ? "Disconnect" : "Connect";
		if (connection_label != connection_button_label) {
			connection_button.set_text(std::string{connection_label});
			connection_button_label = std::string{connection_label};
		}
		connection_button.run();

		for (const auto id : panel.collides(GuiGeometry::Rectangle{GuiGeometry::Position{0, 0},
																   GuiGeometry::Width{800},
																   GuiGeometry::Height{450}})) {
			const auto component = panel.get(id);
			if (!component) {
				throw std::logic_error("Panel collision returned an unknown component");
			}

			std::visit(
				[&grid_colors, &panel](const auto &visible_component) {
					using VisibleComponent = std::remove_cvref_t<decltype(visible_component)>;
					// Components are indexed and queried in panel-local coordinates. The
					// accumulated origin is added only at rendering time, so dragging does
					// not rewrite every component or quadtree entry.
					if constexpr (requires { visible_component.draw(panel.origin()); }) {
						visible_component.draw(panel.origin());
					}

					if constexpr (std::is_same_v<VisibleComponent,
												 KarkinolutionView::Gui::Components::MediumLevel::Grid>) {
						std::size_t color_index = 0;
						for (int row = 0; row < visible_component.rows().value(); ++row) {
							for (int column = 0; column < visible_component.columns().value(); ++column) {
								visible_component.draw_cell(GuiGeometry::Column{column},
															GuiGeometry::Row{row},
															grid_colors[color_index++],
															panel.origin());
							}
						}
					}
				},
				component->get_data());
		}

		KarkinolutionView::Window::end_inside_loop();
	}
	KarkinolutionView::Window::end_after_loop();

	return 0;
}
