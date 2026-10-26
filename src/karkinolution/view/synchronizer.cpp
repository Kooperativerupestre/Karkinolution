#include "karkinolution/view/synchronizer.hpp"

#include "karkinolution/networking/sdk/src/client/client.hpp"

#include <karkinolution/view/synchronizer.hpp>

bool KarkinolutionView::Synchronizer::is_connected() const {
	return client.is_connected();
}

bool KarkinolutionView::Synchronizer::connect() {
	client.connect();

	state.connected = true;
	return true;
}

bool KarkinolutionView::Synchronizer::disconnect() {
	client.disconnect();
	state.connected = false;
	return true;
}

KarkinolutionSDK::Client::ResponseResult KarkinolutionView::Synchronizer::request_and_sync(
	const KarkinolutionSDK::Client::RequestData &data) {
	const auto response = client.request(data);
	world.sync(client);
	return response;
}

void KarkinolutionView::Synchronizer::draw() {
	state_view.draw();
}