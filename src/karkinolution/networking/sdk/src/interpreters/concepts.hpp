#pragma once
#include <concepts>
#include <karkinolution/networking/sdk/src/client/client.hpp>
template <typename T>


concept HasRequest = requires(T &client, const KarkinolutionSDK::Client::ClientRequestData &data) {
	{ client.request(data) } -> std::same_as<KarkinolutionSDK::Client::ResponseResult>;
};