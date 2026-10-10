#pragma once
#include "karkinolution/core/error.hpp"
#include "karkinolution/organism/entities/genetics/genetic.hpp"
#include "karkinolution/terrain/soil.hpp"

#include <cstdint>
#include <karkinolution/organism/entities/creature/ontology.hpp>
#include <karkinolution/terrain/soil.hpp>
#include <string>

namespace Karkinolution {
	namespace Utils {
		namespace EnumsToString {
			inline constexpr std::string DEFAULT_UNKNOWN_VALUE = "NULL";

			inline std::string make_soil_type_string(uint8_t value) {
				switch (value) {
					case static_cast<uint8_t>(SoilTypes::ROCK):
						return "ROCK";

					case static_cast<uint8_t>(SoilTypes::DIRT):
						return "DIRT";
					case static_cast<uint8_t>(SoilTypes::SAND):
						return "SAND";
					case static_cast<uint8_t>(SoilTypes::WATER):
						return "WATER";
					default:
						return DEFAULT_UNKNOWN_VALUE;
				}
			}

			inline std::string make_specie_string(uint8_t value) {
				switch (value) {
					case static_cast<uint8_t>(CreatureSpecies::CRAB):
						return "CRAB";
					case static_cast<uint8_t>(CreatureSpecies::CROCODILE):
						return "CROCODILE";
					case static_cast<uint8_t>(CreatureSpecies::FISH):
						return "FISH";
					case static_cast<uint8_t>(CreatureSpecies::HIPPOPOTAMUS):
						return "HIPPOPOTAMUS";
					default:
						return DEFAULT_UNKNOWN_VALUE;
				}
			}

			inline std::string make_gender_string(uint8_t value) {
				switch (value) {
					case static_cast<uint8_t>(Gender::FEMALE):
						return "FEMALE";
					case static_cast<uint8_t>(Gender::MALE):
						return "MALE";
					default:
						return DEFAULT_UNKNOWN_VALUE;
				}
			}
		} // namespace EnumsToString

	} // namespace Utils
} // namespace Karkinolution
