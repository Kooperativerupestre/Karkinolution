#pragma once

#include "karkinolution/networking/sdk/src/interpreters/concepts.hpp"
#include "karkinolution/networking/sdk/src/interpreters/generic_id.hpp"
#include "karkinolution/organism/entities/genetics/genetic.hpp"

#include <karkinolution/binary/core_aliases.hpp>
#include <karkinolution/binary/frames/analyzer.hpp>
#include <karkinolution/binary/frames/motor.hpp>
#include <karkinolution/binary/interpreters/creature.hpp>
#include <karkinolution/binary/interpreters/models/creature.hpp>
#include <karkinolution/core/id.hpp>
#include <optional>

namespace KarkinolutionSDK {
	namespace Interpreters {
		namespace Creature {
			// Returns std::nullopt when the server responds with "none" (entity not found).
			template <HasRequest T>
			std::optional<Karkinolution::Binary::Interpreters::Models::Creature::Creature>
			see_creature(T &client, BaseIdType id) {
				const auto payload = CreatureSRI::Request::make_see_creature(id);
				const auto built   = Frames::Motor::make_request(
                    RequestSubTypes::SEE,
                    GENERIC_INTERPRETER_ID,
                    RequestResource::CREATURE,
                    std::vector<std::byte>(payload.begin(), payload.end()));

				const KarkinolutionSDK::Client::RequestData data{.sub_type = built.sub_type,
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

			template <HasRequest T>
			std::optional<Karkinolution::Binary::Interpreters::Models::Creature::Creature>
			make_creature(T                                &client,
						  std::optional<Gender>             gender,
						  std::optional<CreatureSpecies>    specie,
						  const std::optional<Vec3>        &position,
						  const std::optional<std::string> &name) {
				const auto payload =
					CreatureSRI::Request::make_create_creature(gender, specie, position, name);
				const auto built = Frames::Motor::make_request(
					RequestSubTypes::CREATE,
					GENERIC_INTERPRETER_ID,
					RequestResource::CREATURE,
					std::vector<std::byte>(payload.begin(), payload.end()));

				const KarkinolutionSDK::Client::RequestData data{.sub_type = built.sub_type,
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
		} // namespace Creature
	} // namespace Interpreters
} // namespace KarkinolutionSDK

namespace CreatureSDKInterpreter = KarkinolutionSDK::Interpreters::Creature;
