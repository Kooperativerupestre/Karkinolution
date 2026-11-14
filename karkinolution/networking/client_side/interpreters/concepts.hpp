#pragma once

#include <concepts>
#include <karkinolution/networking/client_side/client/client.hpp>

namespace Karkinolution::Networking::ClientSide {
	template <typename T>
	concept HasRequest = requires(T &client, const Client::RequestData &data) {
		{ client.request(data) } -> std::same_as<Client::ResponseResult>;
	};

	template <typename T>
	concept HasSystemRequest = requires(T &client, const Client::SystemRequestData &data) {
		{ client.request(data) } -> std::same_as<Protocol::SystemResponseResult>;
	};
} // namespace Karkinolution::Networking::ClientSide
