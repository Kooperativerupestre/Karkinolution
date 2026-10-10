#include "karkinolution/binary/serialization/interpreters/territory/soil/soil.hpp"

#include "karkinolution/binary/byte_range.hpp"
#include "karkinolution/binary/byte_utils.hpp"
#include "karkinolution/binary/serialization/interpreters/identity/alias.hpp"
#include "karkinolution/binary/serialization/interpreters/math/geometry/geometry.hpp"
#include "karkinolution/binary/serialization/interpreters/properties/properties.hpp"
#include "karkinolution/binary/serialization/serializer.hpp"

#include <karkinolution/binary/deserialization/deserializer.hpp>
#include <karkinolution/binary/serialization/interpreters/math/geometry/alias.hpp>
#include <karkinolution/binary/serialization/interpreters/math/vec/alias.hpp>
#include <karkinolution/binary/serialization/interpreters/properties/alias.hpp>
#include <karkinolution/binary/serialization/interpreters/territory/soil/alias.hpp>

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
		Deserializer::append_bytes(bytes, seria_pr, i * PropertiesSRI::Layout::TOTAL_SIZE - 1);
	}
	return bytes;
}

SoilSRI::BytesArray::DamageComponent SoilSRI::serialize_damage(const SoilPiece &soil) {
	SoilSRI::BytesArray::DamageComponent array{};

	if (soil.components.exists<SoilPieceComponents::Damage>()) {
		array[0]           = static_cast<std::byte>(0x01);
		const auto &damage = soil.components.try_get<SoilPieceComponents::Damage>();

		Deserializer::append_bytes(array, Serializer::convert_float(damage->damage), 1);
	}

	return array;
}

SoilSRI::BytesArray::MovementCost SoilSRI::serialize_movement_cost(const SoilPiece &soil) {
	SoilSRI::BytesArray::MovementCost array{};

	if (soil.components.exists<SoilPieceComponents::MovementCost>()) {
		array[0]                  = static_cast<std::byte>(0x01);
		const auto &movement_cost = soil.components.try_get<SoilPieceComponents::MovementCost>();

		Deserializer::append_bytes(array, Serializer::convert_float(movement_cost->cost), 1);
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

	Deserializer::append_bytes(array,
							   serialized_properties,
							   SoilSRI::Layout::PropertiesCount::OFFSET);

	const auto serialized_required_capabilities =
		serialize_required_capabilities(soil.required_capabilities);

	Deserializer::append_bytes(array,
							   serialized_required_capabilities,
							   SoilSRI::Layout::RequiredCapabilitiesCount::OFFSET);

	const auto serialized_damage_c = serialize_damage(soil);
	Deserializer::append_bytes(array, serialized_damage_c, SoilSRI::Layout::HasDamage::OFFSET);
	const auto serialized_movement_cost = serialize_movement_cost(soil);
	Deserializer::append_bytes(array,
							   serialized_movement_cost,
							   SoilSRI::Layout::HasMovementCost::OFFSET);

	const auto serialized_radius = GeometrySRI::serialize_radius(soil.radius);
	Deserializer::append_bytes(array, serialized_radius, SoilSRI::Layout::Radius::OFFSET);

	const auto serialized_position = VecSRI::serialize_vec((soil.position));
	Deserializer::append_bytes(array, serialized_position, SoilSRI::Layout::Position::OFFSET);

	const auto serialized_id = serialize_id(soil);
	Deserializer::append_bytes(array, serialized_id, SoilSRI::Layout::Id::OFFSET);
	return array;
}