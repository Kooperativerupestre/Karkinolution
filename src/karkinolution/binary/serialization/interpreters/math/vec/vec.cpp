#include "karkinolution/binary/serialization/interpreters/math/vec/vec.hpp"

#include "karkinolution/binary/deserialization/deserializer.hpp"
#include "karkinolution/binary/serialization/serializer.hpp"

#include <karkinolution/binary/serialization/interpreters/math/vec/alias.hpp>

VecSRI::BytesArrays::Vec VecSRI::serialize_vec(const Vec3 &vec) {
	VecSRI::BytesArrays::Vec result;

	const auto x = Serializer::convert_double(vec.x);
	Deserializer::append_bytes(result, x, VecSRI::Layout::X::OFFSET);

	const auto y = Serializer::convert_double(vec.y);
	Deserializer::append_bytes(result, y, VecSRI::Layout::Y::OFFSET);

	const auto z = Serializer::convert_double(vec.z);
	Deserializer::append_bytes(result, z, VecSRI::Layout::Z::OFFSET);
	return result;
}
