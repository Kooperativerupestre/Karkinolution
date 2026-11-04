#include "karkinolution/binary/interpreters/vec.hpp"

#include "karkinolution/binary/byte_utils.hpp"
#include "karkinolution/binary/deserializer.hpp"
#include "karkinolution/binary/serializer.hpp"

// SRI implementations
VecSRI::BytesArrays::Vec VecSRI::serialize_vec(const Karkinolution::Math::Vec::Vec3 &vec) {
	VecSRI::BytesArrays::Vec result;

	const auto x = Karkinolution::Binary::Serializer::convert_double(vec.x);
	Karkinolution::Binary::Utils::append_bytes(result, x, VecSRI::Layout::X::OFFSET);

	const auto y = Karkinolution::Binary::Serializer::convert_double(vec.y);
	Karkinolution::Binary::Utils::append_bytes(result, y, VecSRI::Layout::Y::OFFSET);

	const auto z = Karkinolution::Binary::Serializer::convert_double(vec.z);
	Karkinolution::Binary::Utils::append_bytes(result, z, VecSRI::Layout::Z::OFFSET);
	return result;
}

// DSI implementations
double VecDSI::get_x(const std::vector<std::byte> &payload, std::size_t offset) {
	return Karkinolution::Binary::Deserializer::read_double(payload,
															VecSRI::Layout::X::OFFSET + offset);
}

double VecDSI::get_y(const std::vector<std::byte> &payload, std::size_t offset) {
	return Karkinolution::Binary::Deserializer::read_double(payload,
															VecSRI::Layout::Y::OFFSET + offset);
}

double VecDSI::get_z(const std::vector<std::byte> &payload, std::size_t offset) {
	return Karkinolution::Binary::Deserializer::read_double(payload,
															VecSRI::Layout::Z::OFFSET + offset);
}

Karkinolution::Math::Vec::Vec3 VecDSI::get_vec(const std::vector<std::byte> &payload,
											   std::size_t                   offset) {
	return Karkinolution::Math::Vec::Vec3(get_x(payload, offset),
										  get_y(payload, offset),
										  get_z(payload, offset));
}
