#include "karkinolution/core/error.hpp"

#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <gtest/gtest.h>
#include <karkinolution/binary/binary_error.hpp>
#include <karkinolution/binary/deserialization/deserializer.hpp>
#include <karkinolution/binary/serialization/serializer.hpp>
#include <karkinolution/utils/k_random.hpp>
#include <string>
#include <vector>

namespace {
	std::string random_string() {
		static constexpr char CHARSET[] =
			"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

		std::uniform_int_distribution<size_t> length_dist(1, 64);
		std::uniform_int_distribution<size_t> char_dist(0, sizeof(CHARSET) - 2);

		std::string result;
		size_t      length = length_dist(gen);
		result.reserve(length);

		for (size_t i = 0; i < length; i++) {
			result.push_back(CHARSET[char_dist(gen)]);
		}

		return result;
	}

	template <typename Container>
	void append_to_buffer(std::vector<std::byte> &buffer, const Container &bytes) {
		for (const auto &byte : bytes) {
			buffer.push_back(static_cast<std::byte>(byte));
		}
	}

	void append_to_buffer(std::vector<std::byte> &buffer, std::byte byte) {
		buffer.push_back(byte);
	}
} // namespace

// ============================================================================
// High-level API Roundtrip Tests
// ============================================================================

TEST(SerializeDeserializeRoundtrip, HighLevel_AllTypesRoundtrip) {
	for (int iteration = 0; iteration < 10; iteration++) {
		SCOPED_TRACE(::testing::Message() << "iteration " << iteration);

		const auto        original_u32   = RandomGenerators::generate<std::uint32_t>();
		const auto        original_u64   = RandomGenerators::generate<std::uint64_t>();
		const auto        original_dbl   = RandomGenerators::generate<double>();
		const auto        original_u8    = RandomGenerators::generate<std::uint8_t>();
		const std::string original_str   = random_string();
		const auto        original_float = RandomGenerators::generate<float>();

		std::vector<std::byte> buffer;

		const size_t offset_u32 = buffer.size();
		append_to_buffer(buffer, Serializer::convert_uint32_t(original_u32));

		const size_t offset_u64 = buffer.size();
		append_to_buffer(buffer, Serializer::convert_uint64_t(original_u64));

		const size_t offset_dbl = buffer.size();
		append_to_buffer(buffer, Serializer::convert_double(original_dbl));

		const size_t offset_u8 = buffer.size();
		append_to_buffer(buffer, Serializer::convert_uint8_t(original_u8));

		const size_t offset_str = buffer.size();
		append_to_buffer(buffer, Serializer::convert_string(original_str));

		const size_t offset_float = buffer.size();
		append_to_buffer(buffer, Serializer::convert_float(original_float));

		EXPECT_EQ(Deserializer::read_uint32_t(buffer, offset_u32), original_u32);
		EXPECT_EQ(Deserializer::read_uint64_t(buffer, offset_u64), original_u64);
		EXPECT_DOUBLE_EQ(Deserializer::read_double(buffer, offset_dbl), original_dbl);
		EXPECT_EQ(Deserializer::read_uint8_t(buffer, offset_u8), original_u8);
		EXPECT_EQ(Deserializer::read_string(buffer, offset_str, original_str.size()), original_str);
		EXPECT_FLOAT_EQ(Deserializer::read_float(buffer, offset_float), original_float);
	}
}

// ============================================================================
// Core Functions: Exact Byte Order & Value Interpretation Tests
// ============================================================================

TEST(SerializeDeserializeRoundtrip, Core_ByteOrderAndValueInterpretation) {
	// 8 Bytes: 0x0102030405060708ULL -> Big-endian byte representation
	{
		constexpr std::uint64_t test_val = 0x0102030405060708ULL;
		constexpr auto          bytes    = Serializer::Core::convert_8_bytes(test_val);

		EXPECT_EQ(bytes[0], std::byte{0x01});
		EXPECT_EQ(bytes[1], std::byte{0x02});
		EXPECT_EQ(bytes[2], std::byte{0x03});
		EXPECT_EQ(bytes[3], std::byte{0x04});
		EXPECT_EQ(bytes[4], std::byte{0x05});
		EXPECT_EQ(bytes[5], std::byte{0x06});
		EXPECT_EQ(bytes[6], std::byte{0x07});
		EXPECT_EQ(bytes[7], std::byte{0x08});

		constexpr auto deserialized = Deserializer::Core::deserialize_8_bytes(bytes, 0);
		EXPECT_EQ(deserialized, test_val);
	}

	// 4 Bytes: 0x12345678U -> Big-endian byte representation
	{
		constexpr std::uint32_t test_val = 0x12345678U;
		constexpr auto          bytes    = Serializer::Core::convert_4_bytes(test_val);

		EXPECT_EQ(bytes[0], std::byte{0x12});
		EXPECT_EQ(bytes[1], std::byte{0x34});
		EXPECT_EQ(bytes[2], std::byte{0x56});
		EXPECT_EQ(bytes[3], std::byte{0x78});

		constexpr auto deserialized = Deserializer::Core::deserialize_4_bytes(bytes, 0);
		EXPECT_EQ(deserialized, test_val);
	}

	// 2 Bytes: 0xABCD -> Big-endian byte representation
	{
		constexpr std::uint16_t test_val = 0xABCD;
		constexpr auto          bytes    = Serializer::Core::convert_2_bytes(test_val);

		EXPECT_EQ(bytes[0], std::byte{0xAB});
		EXPECT_EQ(bytes[1], std::byte{0xCD});

		constexpr auto deserialized = Deserializer::Core::deserialize_2_bytes(bytes, 0);
		EXPECT_EQ(deserialized, test_val);
	}

	// 1 Byte: 0xEF
	{
		constexpr std::uint8_t test_val = 0xEF;
		constexpr auto         byte     = Serializer::Core::convert_1_byte(test_val);

		EXPECT_EQ(byte, std::byte{0xEF});

		constexpr std::array<std::byte, 1> bytes_arr = {byte};
		constexpr auto deserialized = Deserializer::Core::deserialize_1_byte(bytes_arr, 0);
		EXPECT_EQ(deserialized, std::byte{0xEF});
	}
}

// ============================================================================
// Core Functions: Random Values Roundtrip with Offsets
// ============================================================================

TEST(SerializeDeserializeRoundtrip, Core_RandomValuesRoundtripWithOffsets) {
	for (int iteration = 0; iteration < 10; iteration++) {
		SCOPED_TRACE(::testing::Message() << "iteration " << iteration);

		const auto original_8 = RandomGenerators::generate<std::uint64_t>();
		const auto original_4 = RandomGenerators::generate<std::uint32_t>();
		const auto original_2 = RandomGenerators::generate<std::uint16_t>();
		const auto original_1 = RandomGenerators::generate<std::uint8_t>();

		// Add dummy padding bytes to verify offset interpretation works properly
		std::vector<std::byte> buffer = {std::byte{0xAA}, std::byte{0xBB}, std::byte{0xCC}};

		const size_t offset_8 = buffer.size();
		append_to_buffer(buffer, Serializer::Core::convert_8_bytes(original_8));

		const size_t offset_4 = buffer.size();
		append_to_buffer(buffer, Serializer::Core::convert_4_bytes(original_4));

		const size_t offset_2 = buffer.size();
		append_to_buffer(buffer, Serializer::Core::convert_2_bytes(original_2));

		const size_t offset_1 = buffer.size();
		append_to_buffer(buffer, Serializer::Core::convert_1_byte(original_1));

		EXPECT_EQ(Deserializer::Core::deserialize_8_bytes(buffer, offset_8), original_8);
		EXPECT_EQ(Deserializer::Core::deserialize_4_bytes(buffer, offset_4), original_4);
		EXPECT_EQ(Deserializer::Core::deserialize_2_bytes(buffer, offset_2), original_2);
		EXPECT_EQ(Deserializer::Core::deserialize_1_byte(buffer, offset_1), std::byte{original_1});
	}
}

// ============================================================================
// Core Types: Multi-type Interpretations (Unsigned, Signed, Floating-point)
// ============================================================================

TEST(SerializeDeserializeRoundtrip, Core_TypesInterpretation) {
	// 8-byte interpretation: uint64, int64, double
	{
		constexpr std::int64_t original_i64 = -9223372036854775807LL;
		constexpr auto         bytes_i64 =
			Serializer::Core::convert_8_bytes(static_cast<std::uint64_t>(original_i64));
		Deserializer::Types::Bytes8Deserialized des8_i64(bytes_i64, 0);
		EXPECT_EQ(des8_i64.as_int64_t(), original_i64);

		constexpr double original_dbl = -123456.789012345;
		constexpr auto   dbl_bits     = std::bit_cast<std::uint64_t>(original_dbl);
		constexpr auto   bytes_dbl    = Serializer::Core::convert_8_bytes(dbl_bits);
		Deserializer::Types::Bytes8Deserialized des8_dbl(bytes_dbl, 0);
		EXPECT_DOUBLE_EQ(des8_dbl.as_double(), original_dbl);
		EXPECT_EQ(des8_dbl.as_uint64_t(), dbl_bits);
	}

	// 4-byte interpretation: uint32, int32, float
	{
		constexpr std::int32_t original_i32 = -2147483647;
		constexpr auto         bytes_i32 =
			Serializer::Core::convert_4_bytes(static_cast<std::uint32_t>(original_i32));
		Deserializer::Types::Bytes4Deserialized des4_i32(bytes_i32, 0);
		EXPECT_EQ(des4_i32.as_int32_t(), original_i32);

		constexpr float original_flt = -987.654f;
		constexpr auto  flt_bits     = std::bit_cast<std::uint32_t>(original_flt);
		constexpr auto  bytes_flt    = Serializer::Core::convert_4_bytes(flt_bits);
		Deserializer::Types::Bytes4Deserialized des4_flt(bytes_flt, 0);
		EXPECT_FLOAT_EQ(des4_flt.as_float(), original_flt);
		EXPECT_EQ(des4_flt.as_uint32_t(), flt_bits);
	}

	// 2-byte interpretation: uint16, int16
	{
		constexpr std::int16_t original_i16 = -32767;
		constexpr auto         bytes_i16 =
			Serializer::Core::convert_2_bytes(static_cast<std::uint16_t>(original_i16));
		Deserializer::Types::Bytes2Deserialized des2_i16(bytes_i16, 0);
		EXPECT_EQ(des2_i16.as_int16_t(), original_i16);
		EXPECT_EQ(des2_i16.as_uint16_t(), static_cast<std::uint16_t>(original_i16));
	}

	// 1-byte interpretation: uint8, byte
	{
		constexpr std::uint8_t             original_u8 = 250;
		constexpr auto                     byte_val = Serializer::Core::convert_1_byte(original_u8);
		constexpr std::array<std::byte, 1> bytes_u8 = {byte_val};
		Deserializer::Types::Bytes1Deserialized des1_u8(bytes_u8, 0);
		EXPECT_EQ(des1_u8.as_uint8_t(), original_u8);
		EXPECT_EQ(des1_u8.as_byte(), std::byte{original_u8});
	}
}

// ============================================================================
// Core Functions: Boundary / Error Handling
// ============================================================================

TEST(SerializeDeserializeRoundtrip, Core_InsufficientBufferSizeThrowsError) {
	const std::vector<std::byte> short_buffer = {std::byte{0x01}, std::byte{0x02}};

	// 8 bytes required, but buffer has only 2 bytes
	EXPECT_THROW(Deserializer::Core::deserialize_8_bytes(short_buffer, 0), ByteError);

	// 4 bytes required, but buffer has only 2 bytes
	EXPECT_THROW(Deserializer::Core::deserialize_4_bytes(short_buffer, 0), ByteError);

	// 2 bytes required, but offset 1 leaves only 1 byte
	EXPECT_THROW(Deserializer::Core::deserialize_2_bytes(short_buffer, 1), ByteError);

	// 1 byte required, but offset 2 is out of range
	EXPECT_THROW(Deserializer::Core::deserialize_1_byte(short_buffer, 2), ByteError);

	// Completely out of range offset
	EXPECT_THROW(Deserializer::Core::deserialize_8_bytes(short_buffer, 100), ByteError);
	EXPECT_THROW(Deserializer::Core::deserialize_4_bytes(short_buffer, 100), ByteError);
	EXPECT_THROW(Deserializer::Core::deserialize_2_bytes(short_buffer, 100), ByteError);
	EXPECT_THROW(Deserializer::Core::deserialize_1_byte(short_buffer, 100), ByteError);
}