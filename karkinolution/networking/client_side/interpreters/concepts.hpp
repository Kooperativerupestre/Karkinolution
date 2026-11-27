#pragma once

#include "karkinolution/networking/client_side/client/protocol_session.hpp"

#include <concepts>
#include <karkinolution/networking/client_side/client/client.hpp>

namespace Karkinolution::Networking::ClientSide {
	template <typename T>
	concept CanRequestWorldScope = requires(T &client, const Client::WorldRequestData &data) {
		{ client.request_world_scope(data) } -> std::same_as<Protocol::WorldSession::ResponseResult>;
	};


	template <typename T>
	concept CanRequestSystemScope = requires(T &client, const Client::SystemRequestData &data) {
		{ client.request_system_scope(data) } -> std::same_as<Protocol::SystemSession::ResponseResult>;
	};

	template <typename T>
	concept CanRequest =
		requires(T &client, const Client::SystemRequestData &s_data, const Client::WorldRequestData &w_data) {
			{ client.request_world_scope(w_data) } -> std::same_as<Protocol::WorldSession::ResponseResult>;
			{ client.request_system_scope(s_data) } -> std::same_as<Protocol::SystemSession::ResponseResult>;
		};

	template <typename T>
	concept CanHandleConnection = requires(T &client) {
		{ client.connect() };
		{ client.disconnect() };
		{ client.is_connected() } -> std::same_as<bool>;
	};

	template <typename T>
	concept AsClient = CanRequest<T> && CanHandleConnection<T>;


} // namespace Karkinolution::Networking::ClientSide
