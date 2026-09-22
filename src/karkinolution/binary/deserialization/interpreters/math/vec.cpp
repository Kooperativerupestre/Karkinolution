#include "karkinolution/binary/deserialization/deserializer.hpp"
#include "karkinolution/binary/serialization/interpreters/math/ vec.hpp"

#include <karkinolution/binary/deserialization/interpreters/math/vec.hpp>

Vec3 VecDSI::deserialize_vec(const std::vector<std::byte> &payload, std::size_t offset) {
	const auto x = Deserializer::read_double(payload, VecSRI::TO_GET_X_OFFSET + offset);
	const auto y = Deserializer::read_double(payload, VecSRI::TO_GET_Y_OFFSET + offset);
	const auto z = Deserializer::read_double(payload, VecSRI::TO_GET_Z_OFFSET + offset);

	return Vec3(x, y, z);
}