#include <imgui.h>
#include <karkinolution/view/gui/colors.hpp>
#include <karkinolution/view/window.hpp>
#include <raylib.h>
#include <rlImGui.h>

constexpr ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize
	| ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings
	| ImGuiWindowFlags_NoBackground;

void KarkinolutionView::Window::init() {
	ClearBackground(
		KarkinolutionView::Gui::Colors::translate(Karkinolution::Colors::Presets::Gold));

	rlImGuiBegin();

	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);


	ImGui::Begin("Karkinolution", nullptr, flags);
}