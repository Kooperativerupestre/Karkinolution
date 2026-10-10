#include "karkinolution/binary/serialization/interpreters/math/vec/vec.hpp"

#include "karkinolution/binary/deserialization/deserializer.hpp"
#include "karkinolution/binary/deserialization/interpreters/math/vec/alias.hpp"
#include "karkinolution/binary/deserialization/interpreters/math/vec/vec.hpp"

#include <karkinolution/binary/serialization/interpreters/math/vec/alias.hpp>

double VecDSI::get_x(const std::vector<std::byte> &payload, std::size_t offset) {
	return Deserializer::read_double(payload, VecSRI::Layout::X::OFFSET + offset);
}

double VecDSI::get_y(const std::vector<std::byte> &payload, std::size_t offset) {
	return Deserializer::read_double(payload, VecSRI::Layout::Y::OFFSET + offset);
}

double VecDSI::get_z(const std::vector<std::byte> &payload, std::size_t offset) {
	return Deserializer::read_double(payload, VecSRI::Layout::Z::OFFSET + offset);
}

Vec3 VecDSI::get_vec(const std::vector<std::byte> &payload, std::size_t offset) {
	return Vec3(get_x(payload, offset), get_y(payload, offset), get_z(payload, offset));
}