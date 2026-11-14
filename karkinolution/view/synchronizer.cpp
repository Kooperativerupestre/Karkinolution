#include "karkinolution/view/synchronizer.hpp"

#include "karkinolution/networking/client_side/client/client.hpp"

#include <stdexcept>

bool KarkinolutionView::Synchronizer::is_connected() const {
	return client.is_connected();
}

bool KarkinolutionView::Synchronizer::connect() {
	if (!client.connect()) {
		state.connected = false;
		return false;
	}
	if (!world) {
		world.emplace(client);
	}
	state.connected = true;
	return true;
}

bool KarkinolutionView::Synchronizer::disconnect() {
	client.disconnect();
	state.connected = false;
	return true;
}

Karkinolution::Networking::ClientSide::Client::ResponseResult KarkinolutionView::Synchronizer::request_and_sync(
	const Karkinolution::Networking::ClientSide::Client::RequestData &data) {
	if (!world) {
		throw std::logic_error("Cannot synchronize before the world is initialized");
	}
	const auto response = client.request(data);
	world->sync(client);
	return response;
}

void KarkinolutionView::Synchronizer::draw() {
	state_view.draw();
}
