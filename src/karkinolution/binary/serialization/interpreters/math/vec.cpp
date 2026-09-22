#include "karkinolution/binary/deserialization/deserializer.hpp"
#include "karkinolution/binary/serialization/serializer.hpp"

#include <karkinolution/binary/serialization/interpreters/math/ vec.hpp>

VecSRI::VecBytes VecSRI::serialize_vec(const Vec3 &vec) {
	VecBytes result;

	const auto x = Serializer::convert_double(vec.x);
	Deserializer::append_bytes(result, x, 0);

	const auto y = Serializer::convert_double(vec.y);
	Deserializer::append_bytes(result, y, VecSRI::TO_GET_Y_OFFSET);

	const auto z = Serializer::convert_double(vec.z);
	Deserializer::append_bytes(result, y, TO_GET_Z_OFFSET);
	return result;
}