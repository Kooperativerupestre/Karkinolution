

#include "karkinolution/networking/sdk/src/client/client.hpp"

#include "karkinolution/binary/frames/aliases.hpp"
#include "karkinolution/core/error.hpp"

#include <karkinolution/view/client/client.hpp>

bool KarkinolutionView::Client::Client::is_connected() {
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

KarkinolutionSDK::Client::Client &KarkinolutionView::Client::Client::get_client() {
	return *client;
}

const KarkinolutionSDK::Client::Client &KarkinolutionView::Client::Client::get_client() const {
	return *client;
}