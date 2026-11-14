#pragma once

#include "karkinolution/networking/frame/interpreter.hpp"
#include "karkinolution/networking/sdk/src/interpreters/concepts.hpp"

namespace KarkinolutionSDK::Interpreters::System {
	template <HasSystemRequest T> std::optional<bool> config(T &client, Karkinolution::World::WorldId world_id) {
		const auto payload = Karkinolution::Networking::Frames::Interpreter::SystemCall::SRI::make_config(world_id);
		const KarkinolutionSDK::Client::SystemRequestData request{
			.sub_type = Karkinolution::Networking::Frames::SystemCallSubTypes::CONFIG,
			.payload  = {payload.begin(), payload.end()}};
		const auto result = client.request(request);
		if (!result || !result.value().has_value()) {
			return std::nullopt;
		}
		const auto &output = result.value().value();
		if (output.get_sub_type() != Karkinolution::Networking::Frames::SystemOutputSubTypes::ConfigOutput) {
			throw KarkinolutionSDK::ClientError("Unexpected system output subtype for config");
		}
		return Karkinolution::Networking::Frames::Interpreter::DSI::get_config_output(output.get_payload())
			.could_connect;
	}

	template <HasSystemRequest T> std::vector<Karkinolution::World::WorldId> get_all_world_ids(T &client) {
		const KarkinolutionSDK::Client::SystemRequestData request{
			.sub_type = Karkinolution::Networking::Frames::SystemCallSubTypes::GET_ALL_WORLD_IDS,
			.payload  = {}};
		const auto result = client.request(request);
		if (!result || !result.value().has_value()) {
			return {};
		}
		const auto &output = result.value().value();
		if (output.get_sub_type() != Karkinolution::Networking::Frames::SystemOutputSubTypes::AllWorldIds) {
			throw KarkinolutionSDK::ClientError("Unexpected system output subtype for world IDs");
		}
		return Karkinolution::Networking::Frames::Interpreter::DSI::get_world_ids(output.get_payload());
	}
} // namespace KarkinolutionSDK::Interpreters::System

namespace SystemSDKInterpreter = KarkinolutionSDK::Interpreters::System;
