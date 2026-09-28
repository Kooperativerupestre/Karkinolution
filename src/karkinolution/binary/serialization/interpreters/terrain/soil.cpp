#include "karkinolution/binary/byte_range.hpp"
#include "karkinolution/binary/byte_utils.hpp"
#include "karkinolution/binary/serialization/interpreters/creature.hpp"
#include "karkinolution/binary/serialization/interpreters/math/geometry/geometry.hpp"
#include "karkinolution/binary/serialization/interpreters/math/vec.hpp"
#include "karkinolution/binary/serialization/serializer.hpp"

#include <karkinolution/binary/deserialization/deserializer.hpp>
#include <karkinolution/binary/serialization/interpreters/terrain/soil.hpp>

std::byte SoilSRI::serialize_type(SoilTypes type) {
	if (type == SoilTypes::DIRT) {
		return static_cast<std::byte>(0x01);
	} else if (type == SoilTypes::ROCK) {
		return static_cast<std::byte>(0x02);
	} else if (type == SoilTypes::SAND) {
		return static_cast<std::byte>(0x03);
	} else if (type == SoilTypes::WATER) {
		return static_cast<std::byte>(0x04);
	}
	throw ByteError("Unknown Soil Type");
}

std::byte SoilSRI::serialize_property(SoilProperties property) {
	if (property == SoilProperties::DANGEROUS) {
		return static_cast<std::byte>(0x01);
	}
	throw ByteError("Unknown property of soil");
}

SoilSRI::PropertiesBytesArray
SoilSRI::serialize_properties(const std::vector<SoilProperties> &properties) {
	if (properties.size() > SoilSRI::PROPERTIES_BYTE) {
		throw ByteError(
			std::format("The size of the properties of the soil is bigger than allowed ({})",
						SoilSRI::PROPERTIES_BYTE));
	}
	SoilSRI::PropertiesBytesArray bytes{};

	for (size_t i = 0; i < properties.size(); ++i) {
		bytes[i] = serialize_property(properties[i]);
	}
	return bytes;
}

SoilSRI::RequiredCapabilitiesBytesArray
SoilSRI::serialize_required_capabilities(const std::vector<GenericProperty> &capabilities) {
	if (capabilities.size() > SoilSRI::REQUIRED_CAPABILITIES_BYTES) {
		throw ByteError(
			std::format("The size of required capabilities of the soil is bigger than allowed {}",
						SoilSRI::REQUIRED_CAPABILITIES_BYTES));
	}
	SoilSRI::RequiredCapabilitiesBytesArray bytes{};
	for (size_t i = 0; i < capabilities.size(); ++i) {
		bytes[i] = PropertiesSRI::serialize(capabilities[i]);
	}
	return bytes;
}

SoilSRI::DamageBytesArray SoilSRI::serialize_damage(const SoilPiece &soil) {
	SoilSRI::DamageBytesArray array{};

	if (soil.components.exists<SoilPieceComponents::Damage>()) {
		array[0]           = static_cast<std::byte>(0x01);
		const auto &damage = soil.components.try_get<SoilPieceComponents::Damage>();

		Deserializer::append_bytes(array, Serializer::convert_float(damage->damage), 1);
	}

	return array;
}

SoilSRI::MovementCostBytesArray SoilSRI::serialize_movement_cost(const SoilPiece &soil) {
	SoilSRI::MovementCostBytesArray array{};

	if (soil.components.exists<SoilPieceComponents::MovementCost>()) {
		array[0]                  = static_cast<std::byte>(0x01);
		const auto &movement_cost = soil.components.try_get<SoilPieceComponents::MovementCost>();

		Deserializer::append_bytes(array, Serializer::convert_float(movement_cost->cost), 1);
	}
	return array;
}

SoilSRI::IdBytesArray SoilSRI::serialize_id(const SoilPiece &soil) {
	return Serializer::convert_uint64_t(soil.id);
}

SoilSRI::SoilBytesArray SoilSRI::serialize_soil(const SoilPiece &soil) {
	SoilBytesArray array{};

	array[TO_GET_TYPE_OFFSET] = serialize_type(soil.type);

	const auto serialized_properties = serialize_properties(soil.properties);

	Deserializer::append_bytes(array, serialized_properties, TO_GET_PROPERTIES_OFFSET);

	const auto serialized_required_capabilities =
		serialize_required_capabilities(soil.required_capabilities);

	Deserializer::append_bytes(array,
							   serialized_required_capabilities,
							   TO_GET_REQUIRED_CAPABILITIES_OFFSET);

	const auto serialized_damage_c = serialize_damage(soil);
	Deserializer::append_bytes(array, serialized_damage_c, TO_GET_HAS_DAMAGE_OFFSET);
	const auto serialized_movement_cost = serialize_movement_cost(soil);
	Deserializer::append_bytes(array, serialized_movement_cost, TO_GET_HAS_MOVEMENT_COST_OFFSET);

	const auto serialized_radius = GeometrySRI::serialize_radius(soil.radius);
	Deserializer::append_bytes(array, serialized_radius, TO_GET_RADIUS_OFFSET);

	const auto serialized_position = VecSRI::serialize_vec((soil.position));
	Deserializer::append_bytes(array, serialized_position, TO_GET_POSITION_OFFSET);

	const auto serialized_id = serialize_id(soil);
	Deserializer::append_bytes(array, serialized_id, TO_GET_ID_OFFSET);
	return array;
}