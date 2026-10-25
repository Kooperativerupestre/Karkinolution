#include "karkinolution/core/color.hpp"
#include "karkinolution/networking/sdk/src/client/alias.hpp"
#include "karkinolution/view/client/client.hpp"
#include "karkinolution/view/gui/colors.hpp"
#include "karkinolution/view/gui/components/grid.hpp"
#include "karkinolution/view/gui/geometry/geometry.hpp"
#include "karkinolution/view/screen.hpp"
#include "karkinolution/view/window.hpp"
#include "rlImGui.h"

#include <imgui.h>
#include <karkinolution/view/gui/colors.hpp>
#include <karkinolution/view/presets/aliases.hpp>
#include <karkinolution/view/presets/grid.hpp>
#include <raylib.h>

namespace GuiGeometry = KarkinolutionView::Gui::Geometry;

int main() {
	KPresets::Screen screen_preset{.width = 800, .height = 450};

	InitWindow(screen_preset.width, screen_preset.height, "Karkinolution");

	SetTargetFPS(60);

	rlImGuiSetup(true);

	KarkinolutionView::Client::Client client;

	KarkinolutionView::Screen::State state{.connected = client.is_connected()};

	KarkinolutionView::Screen::StateView view{state};

	while (!WindowShouldClose()) {
		BeginDrawing();

		KarkinolutionView::Window::init();

		state.connected = client.is_connected();

		view.draw();

		if (!state.connected) {
			if (ImGui::Button("Connect")) {
				client.connect();
			}
		} else {
			if (ImGui::Button("Disconnect")) {
				client.disconnect();
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

		ImGui::End();

		rlImGuiEnd();

		EndDrawing();
	}

	rlImGuiShutdown();

	CloseWindow();

	return 0;
}
