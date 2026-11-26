#include "app/presets/screen.hpp"
#include "shared/core/color.hpp"

#include <app/window.hpp>
#include <opsiamethystos/raylib_utils/colors.hpp>
#include <raylib.h>

void KarkinolutionView::Window::init_inside_loop() {
	ClearBackground(Opsiamethystos::Gui::Colors::translate(KShared::Colors::Presets::Cyan));
}

void KarkinolutionView::Window::init_before_loop(KApp::Presets::Screen screen_preset) {
	InitWindow(screen_preset.width, screen_preset.height, "Karkinolution");

	SetTargetFPS(60);
}

void KarkinolutionView::Window::end_inside_loop() {
	EndDrawing();
}

void KarkinolutionView::Window::end_after_loop() {
	CloseWindow();
}
