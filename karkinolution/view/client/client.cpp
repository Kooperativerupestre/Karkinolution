#include "karkinolution/networking/client_side/client/client.hpp"

#include "karkinolution/binary/frames/aliases.hpp"
#include "karkinolution/view/client/client.hpp"
#include "shared/core/error.hpp"

bool KarkinolutionView::Client::Client::is_connected() const {
	return client->is_connected();
}

bool KarkinolutionView::Client::Client::connect() {
	if (client->is_connected()) {
		return true;
	}
	client->connect();
	return client->is_connected();
}

bool KarkinolutionView::Client::Client::disconnect() {
	if (!client->is_connected()) {
		return true;
	}
	client->disconnect();
	return !client->is_connected();
}

Karkinolution::Networking::ClientSide::Client::ResponseResult
KarkinolutionView::Client::Client::request(const Karkinolution::Networking::ClientSide::Client::RequestData &data) {
	return client->request(data);
}
