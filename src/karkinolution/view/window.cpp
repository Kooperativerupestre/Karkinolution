#include "karkinolution/core/color.hpp"
#include "karkinolution/view/presets/screen.hpp"

#include <karkinolution/view/gui/colors.hpp>
#include <karkinolution/view/window.hpp>
#include <raylib.h>

void KarkinolutionView::Window::init_inside_loop() {
	ClearBackground(
		KarkinolutionView::Gui::Colors::translate(Karkinolution::Colors::Presets::Cyan));
}

void KarkinolutionView::Window::init_before_loop(KarkinolutionView::Presets::Screen screen_preset) {
	InitWindow(screen_preset.width, screen_preset.height, "Karkinolution");

	SetTargetFPS(60);
}

void KarkinolutionView::Window::end_inside_loop() {
	EndDrawing();
}

void KarkinolutionView::Window::end_after_loop() {
	CloseWindow();
}