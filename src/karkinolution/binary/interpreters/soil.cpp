#include "karkinolution/binary/interpreters/soil.hpp"

#include "karkinolution/binary/byte_range.hpp"
#include "karkinolution/binary/byte_utils.hpp"
#include "karkinolution/binary/deserializer.hpp"
#include "karkinolution/binary/interpreters/geometry.hpp"
#include "karkinolution/binary/interpreters/identity.hpp"
#include "karkinolution/binary/interpreters/properties.hpp"
#include "karkinolution/binary/interpreters/vec.hpp"
#include "karkinolution/binary/serializer.hpp"
#include "karkinolution/core/error.hpp"
#include "karkinolution/core/id.hpp"
#include "karkinolution/organism/entities/properties/properties.hpp"
#include "karkinolution/terrain/soil.hpp"

#include <cstddef>
#include <format>
#include <magic_enum/magic_enum.hpp>
#include <vector>

// SRI implementations
std::byte SoilSRI::serialize_type(SoilTypes type) {
	return static_cast<std::byte>(static_cast<uint8_t>(type));
}

std::byte SoilSRI::serialize_property(SoilProperties property) {
	if (property == SoilProperties::DANGEROUS) {
		return static_cast<std::byte>(0x01);
	}
	throw ByteError("Unknown property of soil");
}

SoilSRI::BytesArray::Properties
SoilSRI::serialize_properties(const std::vector<SoilProperties> &properties) {
	if (properties.size() > SoilSRI::Layout::MAX_PROPERTIES_SIZE) {
		throw ByteError(
			std::format("The size of the properties of the soil is bigger than allowed ({})",
						SoilSRI::Layout::MAX_PROPERTIES_SIZE));
	}
	SoilSRI::BytesArray::Properties bytes{};
	bytes[0] = Serializer::convert_uint8_t(properties.size());

	for (size_t i = 1; i < properties.size(); ++i) {
		bytes[i] = serialize_property(properties[i]);
	}
	return bytes;
}

SoilSRI::BytesArray::RequiredCapabilities
SoilSRI::serialize_required_capabilities(const std::vector<GenericProperty> &capabilities) {
	if (capabilities.size() > SoilSRI::Layout::MAX_REQUIRED_CAPABILITIES_SIZE) {
		throw ByteError(
			std::format("The size of required capabilities of the soil is bigger than allowed {}",
						SoilSRI::Layout::MAX_REQUIRED_CAPABILITIES_SIZE));
	}
	SoilSRI::BytesArray::RequiredCapabilities bytes{};
	bytes[0] = Serializer::convert_uint8_t(capabilities.size());
	for (size_t i = 1; i < capabilities.size() + 1; ++i) {
		const auto seria_pr = PropertiesSRI::serialize(capabilities[i - 1]);
		BinaryUtils::append_bytes(bytes, seria_pr, i * PropertiesSRI::Layout::TOTAL_SIZE - 1);
	}
	return bytes;
}

SoilSRI::BytesArray::DamageComponent SoilSRI::serialize_damage(const SoilPiece &soil) {
	SoilSRI::BytesArray::DamageComponent array{};

	if (soil.components.exists<SoilPieceComponents::Damage>()) {
		array[0]           = static_cast<std::byte>(0x01);
		const auto &damage = soil.components.try_get<SoilPieceComponents::Damage>();

		BinaryUtils::append_bytes(array, Serializer::convert_float(damage->damage), 1);
	}

	return array;
}

SoilSRI::BytesArray::MovementCost SoilSRI::serialize_movement_cost(const SoilPiece &soil) {
	SoilSRI::BytesArray::MovementCost array{};

	if (soil.components.exists<SoilPieceComponents::MovementCost>()) {
		array[0]                  = static_cast<std::byte>(0x01);
		const auto &movement_cost = soil.components.try_get<SoilPieceComponents::MovementCost>();

		BinaryUtils::append_bytes(array, Serializer::convert_float(movement_cost->cost), 1);
	}
	return array;
}

IdBytes SoilSRI::serialize_id(const SoilPiece &soil) {
	return Serializer::convert_uint64_t(soil.id);
}

SoilSRI::BytesArray::Soil SoilSRI::serialize_soil(const SoilPiece &soil) {
	SoilSRI::BytesArray::Soil array{};

	array[SoilSRI::Layout::Type::OFFSET] = serialize_type(soil.type);

	const auto serialized_properties = serialize_properties(soil.properties);

	BinaryUtils::append_bytes(array,
							  serialized_properties,
							  SoilSRI::Layout::PropertiesCount::OFFSET);

	const auto serialized_required_capabilities =
		serialize_required_capabilities(soil.required_capabilities);

	BinaryUtils::append_bytes(array,
							  serialized_required_capabilities,
							  SoilSRI::Layout::RequiredCapabilitiesCount::OFFSET);

	const auto serialized_damage_c = serialize_damage(soil);
	BinaryUtils::append_bytes(array, serialized_damage_c, SoilSRI::Layout::HasDamage::OFFSET);
	const auto serialized_movement_cost = serialize_movement_cost(soil);
	BinaryUtils::append_bytes(array,
							  serialized_movement_cost,
							  SoilSRI::Layout::HasMovementCost::OFFSET);

	const auto serialized_radius = GeometrySRI::serialize_radius(soil.radius);
	BinaryUtils::append_bytes(array, serialized_radius, SoilSRI::Layout::Radius::OFFSET);

	const auto serialized_position = VecSRI::serialize_vec((soil.position));
	BinaryUtils::append_bytes(array, serialized_position, SoilSRI::Layout::Position::OFFSET);

	const auto serialized_id = serialize_id(soil);
	BinaryUtils::append_bytes(array, serialized_id, SoilSRI::Layout::Id::OFFSET);
	return array;
}

// Request implementations

std::array<std::byte, 8> SoilSRI::Request::make_see_soil(BaseIdType id) {
	return Serializer::serialize(id);
}

// DSI implementations
std::optional<Karkinolution::Binary::Interpreters::Models::Soil::DamageComponent>
SoilDSI::get_damage(const std::vector<std::byte> &payload, std::size_t offset) {
	const auto has_component = Deserializer::read_bool(payload[offset]);

	if (!has_component) {
		return std::nullopt;
	}
	return Karkinolution::Binary::Interpreters::Models::Soil::DamageComponent{
		.value = Deserializer::read_float(payload, offset + SoilSRI::Layout::HasDamage::SIZE)};
}

std::optional<Karkinolution::Binary::Interpreters::Models::Soil::MovementCostComponent>
SoilDSI::get_movement_cost(const std::vector<std::byte> &payload, std::size_t offset) {
	const auto has_component = Deserializer::read_bool(payload[offset]);

	if (!has_component) {
		return std::nullopt;
	}

	return Karkinolution::Binary::Interpreters::Models::Soil::MovementCostComponent{
		.cost = Deserializer::read_float(payload, offset + SoilSRI::Layout::HasMovementCost::SIZE)};
}

BaseIdType SoilDSI::get_id(const std::vector<std::byte> &payload, std::size_t offset) {
	return Deserializer::deserialize<BaseIdType>(payload, offset);
}

Vec3 SoilDSI::get_position(const std::vector<std::byte> &payload, std::size_t offset) {
	return VecDSI::get_vec(payload, offset);
}

SoilTypes SoilDSI::get_type(const std::vector<std::byte> &payload, std::size_t offset) {
	const auto raw_type = Deserializer::read_uint8_t(payload, offset);
	const auto type     = SoilTypes(raw_type);
	if (!magic_enum::enum_contains(type)) {
		throw ByteError(std::format("Invalid soil type {}", raw_type));
	}
	return type;
}

Radius SoilDSI::get_radius(const std::vector<std::byte> &payload, std::size_t offset) {
	return GeometryDSI::deserialize_radius(payload, offset);
}

std::vector<SoilProperties> SoilDSI::get_soil_properties(const std::vector<std::byte> &payload,
														 std::size_t                   offset) {
	std::vector<SoilProperties> properties;

	const auto count = Deserializer::read_uint8_t(payload, offset);

	if (count * PropertiesSRI::Layout::TOTAL_SIZE > SoilSRI::Layout::MAX_PROPERTIES_SIZE) {
		throw ByteError(std::format("The count of properties ({}) of the soil exceeds the max {}",
									count,
									SoilSRI::Layout::MAX_PROPERTIES_SIZE));
	}
	for (std::size_t i = 0; i < count; ++i) {
		properties.push_back(
			SoilProperties(payload[offset + SoilSRI::Layout::PropertiesCount::SIZE + i]));
	}
	return properties;
}

std::vector<GenericProperty> SoilDSI::get_required_properties(const std::vector<std::byte> &payload,
															  std::size_t offset) {

	std::vector<GenericProperty> required_capabilities;

	const auto count = Deserializer::read_uint8_t(payload, offset);
	;
	required_capabilities.reserve(count);

	for (std::size_t i = 0; i < count; ++i) {
		const auto property_offset = offset + SoilSRI::Layout::RequiredCapabilitiesCount::SIZE
			+ i * PropertiesSRI::Layout::TOTAL_SIZE;

		required_capabilities.push_back(PropertiesDSI::deserialize(payload, property_offset));
	}

	return required_capabilities;
}

Karkinolution::Binary::Interpreters::Models::Soil::Soil
SoilDSI::get_soil(const std::vector<std::byte> &payload, std::size_t offset) {
	return Karkinolution::Binary::Interpreters::Models::Soil::Soil{
		get_damage(payload, offset + SoilSRI::Layout::HasDamage::OFFSET),
		get_movement_cost(payload, offset + SoilSRI::Layout::HasMovementCost::OFFSET),
		get_id(payload, offset + SoilSRI::Layout::Id::OFFSET),
		get_radius(payload, offset + SoilSRI::Layout::Radius::OFFSET),
		get_position(payload, offset + SoilSRI::Layout::Position::OFFSET),
		get_type(payload, offset + SoilSRI::Layout::Type::OFFSET),
		get_soil_properties(payload, offset + SoilSRI::Layout::PropertiesCount::OFFSET),
		get_required_properties(payload,
								offset + SoilSRI::Layout::RequiredCapabilitiesCount::OFFSET)};
}

// Request implementations

BaseIdType SoilDSI::Request::interpret_like_get_soil(const std::vector<std::byte> &payload) {
	return Deserializer::deserialize<BaseIdType>(payload, 0);
}