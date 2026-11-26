#include "app/presets/screen.hpp"
#include "shared/core/color.hpp"

#include <app/window.hpp>
#include <opsiamethystos/raylib_utils/colors.hpp>
#include <raylib.h>

void KApp::Window::init_inside_loop() {
	BeginDrawing();
	ClearBackground(Opsiamethystos::Gui::Colors::translate(KShared::Colors::Presets::Cyan));
}

bool KApp::Window::should_window_close() {
	return WindowShouldClose();
}

void KApp::Window::init_before_loop(KApp::Presets::Screen screen_preset) {
	InitWindow(screen_preset.width, screen_preset.height, "Karkinolution");

	SetTargetFPS(60);
}

void KApp::Window::end_inside_loop() {
	EndDrawing();
}

void KApp::Window::end_after_loop() {
	CloseWindow();
}
