#pragma once
#include <concepts>
#include <karkinolution/networking/sdk/src/client/client.hpp>

namespace KarkinolutionSDK {
	template <typename T>

	concept HasRequest = requires(T &client, const KarkinolutionSDK::Client::RequestData &data) {
		{ client.request(data) } -> std::same_as<KarkinolutionSDK::Client::ResponseResult>;
	};

	template <typename T>
	concept HasSystemRequest = requires(T &client, const KarkinolutionSDK::Client::SystemRequestData &data) {
		{ client.request(data) } -> std::same_as<KarkinolutionSDK::Protocol::SystemResponseResult>;
	};
} // namespace KarkinolutionSDK
