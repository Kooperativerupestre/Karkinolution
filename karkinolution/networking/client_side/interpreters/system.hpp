#pragma once

#include "karkinolution/networking/client_side/error.hpp"
#include "karkinolution/networking/client_side/interpreters/concepts.hpp"
#include "karkinolution/networking/frame/system/interpreter.hpp"
#include "karkinolution/networking/frame/system/message_interpreters/system.hpp"

#include <optional>
#include <vector>

namespace Karkinolution::Networking::ClientSide::Interpreters::System {
	template <CanRequestSystemScope T>
	bool create_world(T &client, const Karkinolution::World::WorldPreset &preset, bool auto_connect) {
		const auto payload =
			Karkinolution::Networking::Frames::System::MessageInterpreters::SystemCall::SRI::make_create_world(preset);
		const Client::SystemRequestData request{
			.sub_type = Karkinolution::Networking::Frames::System::SystemCallSubTypes::CREATE_WORLD,
			.payload  = {payload.begin(), payload.end()},
			.extra    = Karkinolution::Networking::Frames::System::Models::SystemCall::CreateWorldExtra{auto_connect}};
		const auto result = client.request_system_scope(request);
		if (!result || !result.value().has_value()) {
			return false;
		}
		const auto &output = result.value().value();
		if (output.get_sub_type() != Karkinolution::Networking::Frames::System::SystemOutputSubTypes::WorldCreated) {
			throw ClientError("Unexpected system output subtype for create world");
		}
		return Karkinolution::Networking::Frames::System::MessageInterpreters::DSI::get_world_created(
				   output.get_payload())
			.world_created;
	}

	template <CanRequestSystemScope T> std::optional<bool> config(T &client, Karkinolution::World::WorldId world_id) {
		const auto payload =
			Karkinolution::Networking::Frames::System::MessageInterpreters::SystemCall::SRI::make_config(world_id);
		const Client::SystemRequestData request{
			.sub_type = Karkinolution::Networking::Frames::System::SystemCallSubTypes::CONFIG,
			.payload  = {payload.begin(), payload.end()}};
		const auto result = client.request_system_scope(request);
		if (!result || !result.value().has_value()) {
			return std::nullopt;
		}
		const auto &output = result.value().value();
		if (output.get_sub_type() != Karkinolution::Networking::Frames::System::SystemOutputSubTypes::ConfigOutput) {
			throw ClientError("Unexpected system output subtype for config");
		}
		return Karkinolution::Networking::Frames::System::Interpreter::DSI::get_config_output(output.get_payload())
			.could_connect;
	}

	template <CanRequestSystemScope T> std::vector<Karkinolution::World::WorldId> get_all_world_ids(T &client) {
		const Client::SystemRequestData request{
			.sub_type = Karkinolution::Networking::Frames::System::SystemCallSubTypes::GET_ALL_WORLD_IDS,
			.payload  = {}};
		const auto result = client.request_system_scope(request);
		if (!result || !result.value().has_value()) {
			return {};
		}
		const auto &output = result.value().value();
		if (output.get_sub_type() != Karkinolution::Networking::Frames::System::SystemOutputSubTypes::AllWorldIds) {
			throw ClientError("Unexpected system output subtype for world IDs");
		}
		return Karkinolution::Networking::Frames::System::Interpreter::DSI::get_world_ids(output.get_payload());
	}
} // namespace Karkinolution::Networking::ClientSide::Interpreters::System

namespace SystemInterpreter = Karkinolution::Networking::ClientSide::Interpreters::System;
