

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

KarkinolutionView::Client::InternalClientGuard
KarkinolutionView::Client::Client::get_client_guard() {
	return {client.value()};
}

void KarkinolutionView::Client::InternalClientGuard::finish() {
	// World::sync(client)
}
