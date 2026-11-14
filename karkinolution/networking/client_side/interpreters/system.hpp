#pragma once

#include "karkinolution/networking/client_side/error.hpp"
#include "karkinolution/networking/client_side/interpreters/concepts.hpp"
#include "karkinolution/networking/frame/interpreter.hpp"

#include <optional>
#include <vector>

namespace Karkinolution::Networking::ClientSide::Interpreters::System {
	template <HasSystemRequest T> std::optional<bool> config(T &client, Karkinolution::World::WorldId world_id) {
		const auto payload = Karkinolution::Networking::Frames::Interpreter::SystemCall::SRI::make_config(world_id);
		const Client::SystemRequestData request{.sub_type =
													Karkinolution::Networking::Frames::SystemCallSubTypes::CONFIG,
												.payload = {payload.begin(), payload.end()}};
		const auto                      result = client.request(request);
		if (!result || !result.value().has_value()) {
			return std::nullopt;
		}
		const auto &output = result.value().value();
		if (output.get_sub_type() != Karkinolution::Networking::Frames::SystemOutputSubTypes::ConfigOutput) {
			throw ClientError("Unexpected system output subtype for config");
		}
		return Karkinolution::Networking::Frames::Interpreter::DSI::get_config_output(output.get_payload())
			.could_connect;
	}

	template <HasSystemRequest T> std::vector<Karkinolution::World::WorldId> get_all_world_ids(T &client) {
		const Client::SystemRequestData request{
			.sub_type = Karkinolution::Networking::Frames::SystemCallSubTypes::GET_ALL_WORLD_IDS,
			.payload  = {}};
		const auto result = client.request(request);
		if (!result || !result.value().has_value()) {
			return {};
		}
		const auto &output = result.value().value();
		if (output.get_sub_type() != Karkinolution::Networking::Frames::SystemOutputSubTypes::AllWorldIds) {
			throw ClientError("Unexpected system output subtype for world IDs");
		}
		return Karkinolution::Networking::Frames::Interpreter::DSI::get_world_ids(output.get_payload());
	}
} // namespace Karkinolution::Networking::ClientSide::Interpreters::System

namespace SystemInterpreter = Karkinolution::Networking::ClientSide::Interpreters::System;
