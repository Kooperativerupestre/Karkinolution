#pragma once
#include "karkinolution/binary/serialization/interpreters/math/geometry/geometry.hpp"
#include "karkinolution/binary/serialization/interpreters/math/vec.hpp"
#include "karkinolution/binary/serialization/interpreters/properties/properties.hpp"
#include "karkinolution/terrain/soil.hpp"

#include <cstdint>

namespace SoilSRI {
	namespace Components {
		inline constexpr std::size_t DamageBytes       = sizeof(float);
		inline constexpr std::size_t MovementCostBytes = sizeof(float);
	} // namespace Components

	/*
	 * Soil protocol
	 *
	 * [1 byte - type]
	 * [7 bytes - properties]
	 * [10 bytes * property bytes (current: 1) - required capabilities]
	 *
	 * [1 byte - has damage]
	 * [4 byte - damage value]
	 * [1 byte - has movement cost]
	 * [4 byte - movement cost]
	 *
	 * [8 bytes - radius]
	 * [24 bytes - position]
	 * [8 bytes - id]
	 */

	inline constexpr std::size_t TYPE_BYTE                   = 1;
	inline constexpr std::size_t PROPERTIES_BYTE             = 7;
	inline constexpr std::size_t REQUIRED_CAPABILITIES_BYTES = 10 * PropertiesSRI::PROPERTY_BYTES;
	inline constexpr std::size_t HAS_DAMAGE_BYTE             = 1;
	inline constexpr std::size_t HAS_MOVEMENT_COST_BYTE      = 1;
	inline constexpr std::size_t ID_BYTES                    = 8;

	using PropertiesBytesArray           = std::array<std::byte, PROPERTIES_BYTE>;
	using RequiredCapabilitiesBytesArray = std::array<std::byte, REQUIRED_CAPABILITIES_BYTES>;
	using DamageBytesArray = std::array<std::byte, HAS_DAMAGE_BYTE + Components::DamageBytes>;
	using MovementCostBytesArray =
		std::array<std::byte, HAS_MOVEMENT_COST_BYTE + Components::MovementCostBytes>;
	using IdBytesArray = std::array<std::byte, ID_BYTES>;


	inline constexpr std::size_t TO_GET_TYPE_OFFSET       = 0;
	inline constexpr std::size_t TO_GET_PROPERTIES_OFFSET = TO_GET_TYPE_OFFSET + 1;

	inline constexpr std::size_t TO_GET_REQUIRED_CAPABILITIES_OFFSET =
		TO_GET_PROPERTIES_OFFSET + PROPERTIES_BYTE;

	inline constexpr std::size_t TO_GET_HAS_DAMAGE_OFFSET =
		TO_GET_REQUIRED_CAPABILITIES_OFFSET + REQUIRED_CAPABILITIES_BYTES;

	inline constexpr std::size_t TO_GET_DAMAGE_OFFSET = TO_GET_HAS_DAMAGE_OFFSET + HAS_DAMAGE_BYTE;

	inline constexpr std::size_t TO_GET_HAS_MOVEMENT_COST_OFFSET =
		TO_GET_DAMAGE_OFFSET + Components::DamageBytes;

	inline constexpr std::size_t TO_GET_MOVEMENT_COST_OFFSET =
		TO_GET_HAS_MOVEMENT_COST_OFFSET + HAS_MOVEMENT_COST_BYTE;

	inline constexpr std::size_t TO_GET_RADIUS_OFFSET =
		TO_GET_MOVEMENT_COST_OFFSET + Components::MovementCostBytes;

	inline constexpr std::size_t TO_GET_POSITION_OFFSET =
		TO_GET_RADIUS_OFFSET + GeometrySRI::RADIUS_BYTES;


	inline constexpr std::size_t TO_GET_ID_OFFSET = TO_GET_POSITION_OFFSET + VecSRI::AXIS_BYTES * 3;

	inline constexpr std::size_t SoilBytes = TO_GET_ID_OFFSET + ID_BYTES;
	using SoilBytesArray                   = std::array<std::byte, SoilBytes>;

	static_assert(SoilBytes == TO_GET_ID_OFFSET + ID_BYTES);

	std::byte            serialize_type(SoilTypes type);
	std::byte            serialize_property(SoilProperties property);
	PropertiesBytesArray serialize_properties(const std::vector<SoilProperties> &properties);


	RequiredCapabilitiesBytesArray
	serialize_required_capabilities(const std::vector<GenericProperty> &capabilities);
	DamageBytesArray       serialize_damage(const SoilPiece &soil);
	MovementCostBytesArray serialize_movement_cost(const SoilPiece &soil);
	IdBytesArray           serialize_id(const SoilPiece &soil);

	SoilBytesArray serialize_soil(const SoilPiece &soil);

} // namespace SoilSRI