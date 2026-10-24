
#include <imgui.h>
#include <karkinolution/view/screen.hpp>

void KarkinolutionView::Screen::StateView::draw() const {
	if (state.connected) {
		ImGui::Text("Connected");
	} else {
		ImGui::Text("Disconnected");
	}
}