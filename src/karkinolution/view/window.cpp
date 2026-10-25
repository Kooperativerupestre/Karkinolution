#include "karkinolution/core/color.hpp"
#include "karkinolution/view/presets/screen.hpp"

#include <imgui.h>
#include <karkinolution/view/gui/colors.hpp>
#include <karkinolution/view/window.hpp>
#include <raylib.h>
#include <rlImGui.h>

constexpr ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize
	| ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings
	| ImGuiWindowFlags_NoBackground;

void KarkinolutionView::Window::init_inside_loop() {
	ClearBackground(
		KarkinolutionView::Gui::Colors::translate(Karkinolution::Colors::Presets::Cyan));

	rlImGuiBegin();

	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);


	ImGui::Begin("Karkinolution", nullptr, flags);
}

void KarkinolutionView::Window::init_before_loop(KarkinolutionView::Presets::Screen screen_preset) {
	InitWindow(screen_preset.width, screen_preset.height, "Karkinolution");

	SetTargetFPS(60);

	rlImGuiSetup(true);
}

void KarkinolutionView::Window::end_inside_loop() {
	ImGui::End();

	rlImGuiEnd();

	EndDrawing();
}

void KarkinolutionView::Window::end_after_loop() {

	rlImGuiShutdown();

	CloseWindow();
}