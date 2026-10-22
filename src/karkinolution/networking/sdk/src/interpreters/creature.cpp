#include "karkinolution/networking/sdk/src/interpreters/creature.hpp"

#include "karkinolution/binary/core_aliases.hpp"
#include "karkinolution/binary/protocol/layouts.hpp"
#include "karkinolution/networking/sdk/src/client/alias.hpp"
#include "karkinolution/networking/sdk/src/client/client.hpp"
#include "karkinolution/networking/sdk/src/interpreters/generic_id.hpp"

#include <karkinolution/binary/frames/analyzer.hpp>
#include <karkinolution/binary/frames/motor.hpp>
#include <karkinolution/binary/interpreters/creature.hpp>

std::optional<CreatureDSI::Models::Creature>
CreatureSDKInterpreter::see_creature(KarkinolutionClient &client, BaseIdType id) {
	const auto payload = CreatureSRI::Request::make_see_creature(id);

	const auto built =
		Frames::Motor::make_request(RequestSubTypes::SEE,
									GENERIC_INTERPRETER_ID,
									RequestResource::CREATURE,
									std::vector<std::byte>(payload.begin(), payload.end()));

	const ClientRequestData data{.sub_type = built.sub_type,
								 .resource = built.resource,
								 .payload  = built.payload};

	const auto result = client.request(data);

	if (!result || !result.value().has_value()) {
		return std::nullopt;
	}


	const auto &frame = result.value().value();

	if (Frames::Analyzer::is_none(frame)) {
		return std::nullopt;
	}

	return CreatureDSI::get_creature(frame.payload, 0);
}

std::optional<CreatureDSI::Models::Creature>
CreatureSDKInterpreter::make_creature(KarkinolutionSDK::Client::Client &client,
									  std::optional<Gender>             gender,
									  std::optional<CreatureSpecies>    specie,
									  const std::optional<Vec3>        &position,
									  const std::optional<std::string> &name) {
	const auto payload = CreatureSRI::Request::make_create_creature(gender, specie, position, name);
	const auto built =
		Frames::Motor::make_request(RequestSubTypes::CREATE,
									GENERIC_INTERPRETER_ID,
									RequestResource::CREATURE,
									std::vector<std::byte>(payload.begin(), payload.end()));

	const ClientRequestData data{.sub_type = built.sub_type,
								 .resource = built.resource,
								 .payload  = built.payload};

	const auto result = client.request(data);
	if (!result || !result.value().has_value()) {
		return std::nullopt;
	}

	const auto &frame = result.value().value();
	if (Frames::Analyzer::is_none(frame)) {
		return std::nullopt;
	}

	return CreatureDSI::get_creature(frame.payload, 0);
}