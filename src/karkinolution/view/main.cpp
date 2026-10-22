#include "karkinolution/view/client/client.hpp"

#include <karkinolution/view/presets/aliases.hpp>
#include <karkinolution/view/presets/grid.hpp>
#include <raygui.h>
#include <raylib.h>

int main() {
	KPresets::Screen screen_preset{.width = 800, .height = 450};

	InitWindow(screen_preset.width, screen_preset.height, "Karkinolution");
	SetTargetFPS(60);

	KarkinolutionView::Client::Client client;

	while (!WindowShouldClose()) {
		BeginDrawing();
		EndDrawing();
	}


	CloseWindow();
	return 0;
}
