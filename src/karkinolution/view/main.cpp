#include "karkinolution/core/color.hpp"
#include "karkinolution/networking/sdk/src/client/alias.hpp"
#include "karkinolution/view/client/client.hpp"
#include "karkinolution/view/gui/colors.hpp"
#include "karkinolution/view/screen.hpp"
#include "rlImGui.h"

#include <imgui.h>
#include <karkinolution/view/gui/colors.hpp>
#include <karkinolution/view/presets/aliases.hpp>
#include <karkinolution/view/presets/grid.hpp>
#include <raylib.h>

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

		ClearBackground(
			KarkinolutionView::Gui::Colors::translate(Karkinolution::Colors::Presets::Gold));

		rlImGuiBegin();

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

		rlImGuiEnd();

		EndDrawing();
	}

	rlImGuiShutdown();

	CloseWindow();

	return 0;
}