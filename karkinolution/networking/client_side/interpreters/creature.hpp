#pragma once

#include "karkinolution/networking/client_side/interpreters/concepts.hpp"
#include "karkinolution/networking/client_side/interpreters/generic_id.hpp"
#include "karkinolution/networking/client_side/interpreters/internal_aliases.hpp"
#include "karkinolution/organism/entities/genetics/genetic.hpp"

#include <karkinolution/networking/frame/world/analyzer.hpp>
#include <karkinolution/networking/frame/world/core_aliases.hpp>
#include <karkinolution/networking/frame/world/interpreters/creature.hpp>
#include <karkinolution/networking/frame/world/interpreters/models/creature.hpp>
#include <karkinolution/networking/frame/world/motor.hpp>
#include <optional>
#include <shared/binary/deserializer.hpp>
#include <shared/binary/types/list.hpp>
#include <shared/core/id.hpp>

namespace Karkinolution::Networking::ClientSide::Interpreters::Creature {
	template <CanRequestWorldScope T>
	std::vector<KShared::Core::Id::BaseIdType> see_all_creatures(T &client) {
		const auto payload = CreatureSRI::Request::make_see_all_creatures();
		const auto built = Karkinolution::Networking::Frames::World::Motor::make_request(
			RequestSubTypes::SEE,
			GENERIC_INTERPRETER_ID,
			Resource::CREATURE,
			std::vector<std::byte>(payload.begin(), payload.end()),
			SeeExtra{.target = SeeTarget::ALL});
		const Client::WorldRequestData data{.sub_type = built.get_sub_type(),
											.resource = built.get_resource(),
											.payload = built.get_payload(),
											.extra = built.get_extra()};
		const auto result = client.request_world_scope(data);
		if (!result || !result.value().has_value()) {
			return {};
		}
		const auto &frame = require_response_frame(result.value().value());
		if (Karkinolution::Networking::Frames::World::Analyzer::is_none(frame)) {
			return {};
		}
		return ListTDSI::deserialize<sizeof(KShared::Core::Id::BaseIdType)>(
			frame.get_payload(),
			[](const std::vector<std::byte> &bytes, std::size_t offset) {
				return KShared::Binary::Deserializer::deserialize<KShared::Core::Id::BaseIdType>(bytes, offset);
			},
			0);
	}

	// Returns std::nullopt when the server responds with "none" (entity not found).
	template <CanRequestWorldScope T>
	std::optional<Karkinolution::Networking::Frames::World::Interpreters::Models::Creature::Creature>
	see_creature(T &client, KShared::Core::Id::BaseIdType id) {
		const auto payload = CreatureSRI::Request::make_see_creature(id);
		const auto built   = Karkinolution::Networking::Frames::World::Motor::make_request(
            RequestSubTypes::SEE,
            GENERIC_INTERPRETER_ID,
            Resource::CREATURE,
            std::vector<std::byte>(payload.begin(), payload.end()),
            SeeExtra{.target = SeeTarget::ONE});

		const Client::WorldRequestData data{.sub_type = built.get_sub_type(),
											.resource = built.get_resource(),
											.payload  = built.get_payload(),
											.extra    = built.get_extra()};

		const auto result = client.request_world_scope(data);
		if (!result || !result.value().has_value()) {
			return std::nullopt;
		}

		const auto &frame = require_response_frame(result.value().value());
		if (Karkinolution::Networking::Frames::World::Analyzer::is_none(frame)) {
			return std::nullopt;
		}

		return CreatureDSI::get_creature(frame.get_payload(), 0);
	}

	template <CanRequestWorldScope T>
	std::optional<Karkinolution::Networking::Frames::World::Interpreters::Models::Creature::Creature>
	make_creature(T                                                                           &client,
				  std::optional<Karkinolution::Organisms::Entities::Creature::Gender>          gender,
				  std::optional<Karkinolution::Organisms::Entities::Creature::CreatureSpecies> specie,
				  const std::optional<KShared::Math::Vec::Vec2>                               &position,
				  const std::optional<std::string>                                            &name) {
		const auto payload = CreatureSRI::Request::make_create_creature(gender, specie, position, name);
		const auto built   = Karkinolution::Networking::Frames::World::Motor::make_request(
            RequestSubTypes::CREATE,
            GENERIC_INTERPRETER_ID,
            Resource::CREATURE,
            std::vector<std::byte>(payload.begin(), payload.end()),
            CreateExtra{});

		const Client::WorldRequestData data{.sub_type = built.get_sub_type(),
											.resource = built.get_resource(),
											.payload  = built.get_payload(),
											.extra    = built.get_extra()};

		const auto result = client.request_world_scope(data);
		if (!result || !result.value().has_value()) {
			return std::nullopt;
		}

		const auto &frame = require_response_frame(result.value().value());
		if (Karkinolution::Networking::Frames::World::Analyzer::is_none(frame)) {
			return std::nullopt;
		}

		return CreatureDSI::get_creature(frame.get_payload(), 0);
	}

	template <CanRequestWorldScope T>
	std::optional<Karkinolution::Networking::Frames::World::Interpreters::Models::Creature::Creature>
	tick_creature(T &client, KShared::Core::Id::BaseIdType id) {
		const auto payload = CreatureSRI::Request::make_see_creature(id);
		const auto built   = Karkinolution::Networking::Frames::World::Motor::make_request(
            RequestSubTypes::CYCLE,
            GENERIC_INTERPRETER_ID,
            Resource::CREATURE,
            std::vector<std::byte>(payload.begin(), payload.end()),
            CycleExtra{});

		const Client::WorldRequestData data{.sub_type = built.get_sub_type(),
											.resource = built.get_resource(),
											.payload  = built.get_payload(),
											.extra    = built.get_extra()};

		const auto result = client.request_world_scope(data);
		if (!result || !result.value().has_value()) {
			return std::nullopt;
		}

		const auto &frame = require_response_frame(result.value().value());
		if (Karkinolution::Networking::Frames::World::Analyzer::is_none(frame)) {
			return std::nullopt;
		}

		return CreatureDSI::get_creature(frame.get_payload(), 0);
	}

	template <CanRequestWorldScope T> bool erase_creature(T &client, KShared::Core::Id::BaseIdType id) {
		const auto payload = CreatureSRI::Request::make_see_creature(id);
		const auto built   = Karkinolution::Networking::Frames::World::Motor::make_request(
            RequestSubTypes::ERASE,
            GENERIC_INTERPRETER_ID,
            Resource::CREATURE,
            std::vector<std::byte>(payload.begin(), payload.end()),
            EraseExtra{});
		const Client::WorldRequestData data{.sub_type = built.get_sub_type(),
											.resource = built.get_resource(),
											.payload  = built.get_payload(),
											.extra    = built.get_extra()};
		const auto                     result = client.request_world_scope()(data);
		if (!result || !result.value().has_value()) {
			return false;
		}
		const auto &frame = require_response_frame(result.value().value());
		if (frame.get_sub_type() != Frames::World::ResponseSubTypes::BOOLEAN || frame.get_payload().size() != 1) {
			throw ClientError("Unexpected response for erasing creature");
		}
		return KShared::Binary::Deserializer::read_bool(frame.get_payload()[0]);
	}
} // namespace Karkinolution::Networking::ClientSide::Interpreters::Creature

namespace CreatureInterpreter = Karkinolution::Networking::ClientSide::Interpreters::Creature;
