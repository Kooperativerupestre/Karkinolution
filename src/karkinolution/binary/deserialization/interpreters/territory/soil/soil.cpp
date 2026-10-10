#include "karkinolution/binary/deserialization/interpreters/territory/soil/soil.hpp"

#include "karkinolution/binary/deserialization/interpreters/math/geometry/geometry.hpp"
#include "karkinolution/binary/deserialization/interpreters/math/vec/alias.hpp"
#include "karkinolution/binary/serialization/interpreters/properties/properties.hpp"
#include "karkinolution/binary/serialization/interpreters/territory/soil/soil.hpp"
#include "karkinolution/core/id.hpp"
#include "karkinolution/organism/entities/properties/properties.hpp"
#include "karkinolution/terrain/soil.hpp"

#include <cstddef>
#include <karkinolution/binary/deserialization/interpreters/math/geometry/alias.hpp>
#include <karkinolution/binary/deserialization/interpreters/properties/alias.hpp>
#include <karkinolution/binary/deserialization/interpreters/territory/soil/alias.hpp>
#include <karkinolution/binary/serialization/interpreters/territory/soil/alias.hpp>
#include <magic_enum/magic_enum.hpp>
#include <vector>

// Prefix D = Deserialized


std::optional<SoilDSI::Models::DamageComponent>
SoilDSI::get_damage(const std::vector<std::byte> &payload, std::size_t offset) {
	const auto has_component =
		Deserializer::read_bool(payload[SoilSRI::Layout::HasDamage::OFFSET + offset]);

	if (!has_component) {
		return std::nullopt;
	}
	return Models::DamageComponent{
		.value = Deserializer::read_float(payload, SoilSRI::Layout::Damage::OFFSET + offset)};
}

std::optional<SoilDSI::Models::MovementCostComponent>
SoilDSI::get_movement_cost(const std::vector<std::byte> &payload, std::size_t offset) {
	const auto has_component =
		Deserializer::read_bool(payload[SoilSRI::Layout::HasMovementCost::OFFSET + offset]);

	if (!has_component) {
		return std::nullopt;
	}

	return Models::MovementCostComponent{
		.cost = Deserializer::read_float(payload, SoilSRI::Layout::MovementCost::OFFSET + offset)};
}

BaseIdType SoilDSI::get_id(const std::vector<std::byte> &payload, std::size_t offset) {
	return Deserializer::deserialize<BaseIdType>(payload, SoilSRI::Layout::Id::OFFSET + offset);
}

Vec3 SoilDSI::get_position(const std::vector<std::byte> &payload, std::size_t offset) {
	return VecDSI::get_vec(payload, SoilSRI::Layout::Position::OFFSET + offset);
}

SoilTypes SoilDSI::get_type(const std::vector<std::byte> &payload, std::size_t offset) {
	const auto raw_type =
		Deserializer::read_uint8_t(payload, SoilSRI::Layout::Type::OFFSET + offset);
	const auto type = SoilTypes(raw_type);
	if (!magic_enum::enum_contains(type)) {
		throw ByteError(std::format("Invalid soil type {}", raw_type));
	}
	return type;
}

Radius SoilDSI::get_radius(const std::vector<std::byte> &payload, std::size_t offset) {
	return GeometryDSI::deserialize_radius(payload, SoilSRI::Layout::Radius::OFFSET + offset);
}

std::vector<SoilProperties> SoilDSI::get_soil_properties(const std::vector<std::byte> &payload,
														 std::size_t                   offset) {
	std::vector<SoilProperties> properties;

	const auto count =
		Deserializer::read_uint8_t(payload, SoilSRI::Layout::PropertiesCount::OFFSET + offset);

	if (count * PropertiesSRI::Layout::TOTAL_SIZE > SoilSRI::Layout::MAX_PROPERTIES_SIZE) {
		throw ByteError(std::format("The count of properties ({}) of the soil exceeds the max {}",
									count,
									SoilSRI::Layout::MAX_PROPERTIES_SIZE));
	}
	for (std::size_t i = 0; i < count; ++i) {
		properties.push_back(
			SoilProperties(payload[SoilSRI::Layout::PropertiesData::OFFSET + offset + i]));
	}
	return properties;
}

std::vector<GenericProperty> SoilDSI::get_required_properties(const std::vector<std::byte> &payload,
															  std::size_t offset) {

	std::vector<GenericProperty> required_capabilities;

	const auto count =
		Deserializer::read_uint8_t(payload,
								   SoilSRI::Layout::RequiredCapabilitiesCount::OFFSET + offset);
	;
	required_capabilities.reserve(count);

	for (std::size_t i = 0; i < count; ++i) {
		const auto property_offset = SoilSRI::Layout::RequiredCapabilitiesData::OFFSET + offset
			+ i * PropertiesSRI::Layout::TOTAL_SIZE;

		required_capabilities.push_back(PropertiesDSI::deserialize(payload, property_offset));
	}

	return required_capabilities;
}

SoilDSI::Models::Soil SoilDSI::get_soil(const std::vector<std::byte> &payload, std::size_t offset) {


	return Models::Soil{get_damage(payload, offset),
						get_movement_cost(payload, offset),
						get_id(payload, offset),
						get_radius(payload, offset),
						get_position(payload, offset),
						get_type(payload, offset),
						get_soil_properties(payload, offset),
						get_required_properties(payload, offset)};
}