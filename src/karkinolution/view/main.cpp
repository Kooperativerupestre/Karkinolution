#include "karkinolution/view/gui/components/aliases.hpp"
#include "karkinolution/view/presets/screen.hpp"

#include <karkinolution/view/presets/aliases.hpp>
#include <karkinolution/view/presets/grid.hpp>
#include <raygui.h>
#include <raylib.h>

int main() {
	KPresets::Screen screen_preset{.width = 800, .height = 450};

	InitWindow(screen_preset.width, screen_preset.height, "Karkinolution");
	SetTargetFPS(60);


	KPresets::Grid grid_preset{.cell_size = 25, .columns = 10, .rows = 10};
	int            creature_count = 0;

	while (!WindowShouldClose()) {
		BeginDrawing();

		if (GuiButton(Rectangle{300, 200, 200, 50}, "add creature")) {
			creature_count++;
		}

		KGComponents::Counter counter{creature_count, Vector2{200, 200}};
		counter.draw();

		EndDrawing();
	}


	CloseWindow();
	return 0;
}
