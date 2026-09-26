#pragma once
#include "karkinolution/organism/entities/genetics/genetic.hpp"

#include <karkinolution/binary/deserialization/interpreters/models.hpp>
#include <karkinolution/core/id.hpp>
#include <vector>

namespace CreatureResponseDSI {
	Gender                get_gender(const std::vector<std::byte> &payload);
	CreatureSpecies       get_specie(const std::vector<std::byte> &payload);
	double                get_position_x(const std::vector<std::byte> &payload);
	double                get_position_y(const std::vector<std::byte> &payload);
	double                get_position_z(const std::vector<std::byte> &payload);
	Vec3                  get_position(const std::vector<std::byte> &payload);
	std::string           get_name(const std::vector<std::byte> &payload);
	DesserializedCreature get_creature(const std::vector<std::byte> &payload);
} // namespace CreatureResponseDSI

namespace CreatureRequestDSI {
	BaseIdType interpret_like_get_creature(const std::vector<std::byte> &payload);
} // namespace CreatureRequestDSI
