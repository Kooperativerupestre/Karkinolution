#include "karkinolution/core/color.hpp"
#include "karkinolution/networking/sdk/src/client/alias.hpp"
#include "karkinolution/view/client/client.hpp"
#include "karkinolution/view/gui/colors.hpp"
#include "karkinolution/view/gui/components/button.hpp"
#include "karkinolution/view/gui/components/grid.hpp"
#include "karkinolution/view/gui/geometry/geometry.hpp"
#include "karkinolution/view/screen.hpp"
#include "karkinolution/view/synchronizer.hpp"
#include "karkinolution/view/window.hpp"
#include "rlImGui.h"

#include <imgui.h>
#include <karkinolution/view/gui/colors.hpp>
#include <karkinolution/view/presets/aliases.hpp>
#include <karkinolution/view/presets/grid.hpp>
#include <raylib.h>

namespace GuiGeometry   = KarkinolutionView::Gui::Geometry;
namespace GuiComponents = KarkinolutionView::Gui::Components;
namespace Colors        = Karkinolution::Colors;
using KarkinolutionView::Synchronizer;

int main() {
	KPresets::Screen screen_preset{.width = 800, .height = 450};

	KarkinolutionView::Window::init_before_loop(screen_preset);

	Synchronizer synchronizer;

	const GuiComponents::Button connect_button{
		.label = "Connect",
		.style =
			GuiComponents::ButtonStyle::from_base(Colors::Presets::Black, Colors::Presets::Yellow)};

	const GuiComponents::Button disconnect_button{
		.label = "Disconnect",
		.style =
			GuiComponents::ButtonStyle::from_base(Colors::Presets::Black, Colors::Presets::Yellow)};

	while (!WindowShouldClose()) {
		BeginDrawing();

		KarkinolutionView::Window::init_inside_loop();


		synchronizer.draw();

		if (!synchronizer.is_connected()) {
			if (connect_button.draw()) {
				synchronizer.connect();
			}
		} else {
			if (disconnect_button.draw()) {
				synchronizer.disconnect();
			}
		}

		KarkinolutionView::Gui::Components::Grid grid{
			.color           = Karkinolution::Colors::Presets::Red,
			.position        = KarkinolutionView::Gui::Geometry::Position{400, 225},
			.thickness       = KarkinolutionView::Gui::Geometry::Thickness{5},
			.width           = KarkinolutionView::Gui::Geometry::Width{150},
			.height          = KarkinolutionView::Gui::Geometry::Height{150},
			.max_cell_width  = KarkinolutionView::Gui::Geometry::Width{25},
			.max_cell_height = KarkinolutionView::Gui::Geometry::Height{25}};

		grid.draw();


		grid.draw_cell(GuiGeometry::Column{0},
					   GuiGeometry::Row{0},
					   Karkinolution::Colors::Presets::Blue);

		grid.draw_cell(GuiGeometry::Column{0},
					   GuiGeometry::Row{1},
					   Karkinolution::Colors::Presets::Crimson);

		KarkinolutionView::Window::end_inside_loop();
	}
	KarkinolutionView::Window::end_after_loop();

	return 0;
}
