#include "shared/binary/interpreters/vec.hpp"

#include "shared/binary/byte_utils.hpp"
#include "shared/binary/deserializer.hpp"
#include "shared/binary/serializer.hpp"

// SRI implementations
VecSRI::BytesArrays::Vec VecSRI::serialize_vec(const KShared::Math::Vec::Vec2 &vec) {
	VecSRI::BytesArrays::Vec result;

	const auto x = KShared::Binary::Serializer::convert_double(vec.x);
	KShared::Binary::Utils::append_bytes(result, x, VecSRI::Layout::X::OFFSET);

	const auto y = KShared::Binary::Serializer::convert_double(vec.y);
	KShared::Binary::Utils::append_bytes(result, y, VecSRI::Layout::Y::OFFSET);

	return result;
}

// DSI implementations
double VecDSI::get_x(const std::vector<std::byte> &payload, std::size_t offset) {
	return KShared::Binary::Deserializer::read_double(payload, VecSRI::Layout::X::OFFSET + offset);
}

double VecDSI::get_y(const std::vector<std::byte> &payload, std::size_t offset) {
	return KShared::Binary::Deserializer::read_double(payload, VecSRI::Layout::Y::OFFSET + offset);
}

KShared::Math::Vec::Vec2 VecDSI::get_vec(const std::vector<std::byte> &payload, std::size_t offset) {
	return KShared::Math::Vec::Vec2(get_x(payload, offset), get_y(payload, offset));
}
