#include "karkinolution/view/synchronizer.hpp"

bool KarkinolutionView::Synchronizer::is_connected() const {
	return client.is_connected();
}

bool KarkinolutionView::Synchronizer::connect() {
	if (!client.connect()) {
		state.connected = false;
		return false;
	}
	state.connected = true;
	return true;
}

bool KarkinolutionView::Synchronizer::disconnect() {
	client.disconnect();
	state.connected = false;
	return true;
}

void KarkinolutionView::Synchronizer::draw() {
	state_view.draw();
}
