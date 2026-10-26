
#include <imgui.h>
#include <karkinolution/view/screen.hpp>

void KarkinolutionView::Screen::StateView::draw() {
	if (state.connected) {
		connection_status.text = "Connected";
	} else {
		connection_status.text = "Disconnected";
	}
	connection_status.draw();
}