#include <cstddef>
#include <shared/binary/serializer.hpp>

namespace KShared::Binary::Serializer {

	using Core::EightBytes;
	using Core::FourBytes;
	using Core::TwoBytes;

	FourBytes convert_uint32_t(std::uint32_t value) {
		return Core::convert_4_bytes(value);
	}

	TwoBytes convert_uint16_t(std::uint16_t value) {
		return Core::convert_2_bytes(value);
	}

	std::byte convert_uint8_t(std::uint8_t value) {
		return Core::convert_1_byte(value);
	}

	EightBytes convert_uint64_t(std::uint64_t value) {
		return Core::convert_8_bytes(value);
	}

	std::vector<std::byte> convert_string(const std::string &value) {
		std::vector<std::byte> bytes;
		bytes.reserve(value.size());

		for (char character : value) {
			bytes.push_back(static_cast<std::byte>(character));
		}

		return bytes;
	}

	FourBytes convert_int(int value) {
		return Core::convert_4_bytes(std::bit_cast<std::uint32_t>(value));
	}

	EightBytes convert_double(double value) {
		return Core::convert_8_bytes(std::bit_cast<std::uint64_t>(value));
	}

	FourBytes convert_float(float value) {
		return Core::convert_4_bytes(std::bit_cast<std::uint32_t>(value));
	}

	std::byte convert_int8_t(std::int8_t value) {
		return Core::convert_1_byte(value);
	}

	TwoBytes convert_int16_t(std::int16_t value) {
		return Core::convert_2_bytes(value);
	}

	FourBytes convert_int32_t(std::int32_t value) {
		return Core::convert_4_bytes(value);
	}

	EightBytes convert_int64_t(std::int64_t value) {
		return Core::convert_8_bytes(value);
	}

	std::byte convert_bool(bool value) {
		if (value) {
			return std::byte(0x01);
		} else {
			return std::byte(0x00);
		}
	}

} // namespace KShared::Binary::Serializer
