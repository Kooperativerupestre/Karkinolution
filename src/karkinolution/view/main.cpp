#include "karkinolution/view/presets/screen.hpp"

#include <karkinolution/view/presets/aliases.hpp>
#include <karkinolution/view/presets/grid.hpp>
#include <raylib.h>

int main() {
	ScreenPreset screen_preset{.width = 800, .height = 450};

	InitWindow(screen_preset.width, screen_preset.height, "Karkinolution");
	SetTargetFPS(60);

	Camera2D camera{.offset =
						Vector2{.x = screen_preset.width / 2.0f, .y = screen_preset.height / 2.0f},
					.target   = {0, 0},
					.rotation = 0,
					.zoom     = 1};

	GridPreset grid_preset{.cell_size = 25, .columns = 10, .rows = 10};
	while (!WindowShouldClose()) {
		BeginDrawing();
		BeginMode2D(camera);

		ClearBackground(RAYWHITE);
		for (int y = 0; y < grid_preset.rows; y++) {

			for (int x = 0; x < grid_preset.columns; x++) {
				DrawRectangleLinesEx(Rectangle(x * grid_preset.cell_size + grid_preset.x_offset(),
											   y * grid_preset.cell_size + grid_preset.y_offset(),
											   grid_preset.cell_size,
											   grid_preset.cell_size),
									 3.0f,
									 RED);
			}
		}
		EndMode2D();
		EndDrawing();
	}


	CloseWindow();
	return 0;
}
