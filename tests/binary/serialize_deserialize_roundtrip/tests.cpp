#include "karkinolution/core/error.hpp"

#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <gtest/gtest.h>
#include <karkinolution/binary/binary_error.hpp>
#include <karkinolution/binary/byte_utils.hpp>
#include <karkinolution/binary/deserialization/deserializer.hpp>
#include <karkinolution/binary/deserialization/interpreters/corpse.hpp>
#include <karkinolution/binary/deserialization/interpreters/math/geometry/geometry.hpp>
#include <karkinolution/binary/deserialization/interpreters/math/stats/stats.hpp>
#include <karkinolution/binary/deserialization/interpreters/math/unit/unit.hpp>
#include <karkinolution/binary/deserialization/interpreters/math/vec.hpp>
#include <karkinolution/binary/deserialization/interpreters/properties/properties.hpp>
#include <karkinolution/binary/deserialization/interpreters/territory/territory.hpp>
#include <karkinolution/binary/serialization/interpreters/corpse.hpp>
#include <karkinolution/binary/serialization/interpreters/math/geometry/geometry.hpp>
#include <karkinolution/binary/serialization/interpreters/math/stats/stats.hpp>
#include <karkinolution/binary/serialization/interpreters/math/unit/unit.hpp>
#include <karkinolution/binary/serialization/interpreters/math/vec.hpp>
#include <karkinolution/binary/serialization/interpreters/properties/properties.hpp>
#include <karkinolution/binary/serialization/interpreters/terrain/soil.hpp>
#include <karkinolution/binary/serialization/interpreters/territory/territory.hpp>
#include <karkinolution/binary/serialization/serializer.hpp>
#include <karkinolution/math/geometry/models.hpp>
#include <karkinolution/math/physic/vec/model.hpp>
#include <karkinolution/math/stats/runtime_values.hpp>
#include <karkinolution/organism/entities/corpse/corpse.hpp>
#include <karkinolution/organism/entities/properties/properties.hpp>
#include <karkinolution/terrain/soil.hpp>
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

// High-level API Roundtrip Tests

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

// Core Functions: Exact Byte Order & Value Interpretation Tests

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

// Core Functions: Random Values Roundtrip with Offsets

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

// Core Types: Multi-type Interpretations (Unsigned, Signed, Floating-point)

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

// Core Functions: Boundary / Error Handling

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

// Math: Vec

TEST(MathSerializeDeserializeRoundtrip, Vec3) {
	for (int iteration = 0; iteration < 10; iteration++) {
		SCOPED_TRACE(::testing::Message() << "iteration " << iteration);

		const Vec3 original = RandomGenerators::generate<Vec3>();

		const auto                   vec_bytes = VecSRI::serialize_vec(original);
		const std::vector<std::byte> buffer(vec_bytes.begin(), vec_bytes.end());

		const Vec3 result = VecDSI::deserialize_vec(buffer, 0);

		EXPECT_EQ(result, original);
	}
}

// Math: Stats - Full Value Roundtrip

TEST(MathSerializeDeserializeRoundtrip, Stats_FullValueRoundtrip) {
	// GenericRuntimeValue<double>
	{
		GenericRuntimeValue<double>  stat(50.0, 100.0, 0.0);
		const auto                   bytes = StatsSRI::serialize_limited_value(stat);
		const std::vector<std::byte> buffer(bytes.begin(), bytes.end());

		const auto deserialized = StatsDSI::get_stats<double>(buffer);
		EXPECT_DOUBLE_EQ(deserialized.value, stat.value());
		EXPECT_DOUBLE_EQ(deserialized.max, stat.max());
		EXPECT_DOUBLE_EQ(deserialized.min, stat.min());
	}

	// GenericRuntimeValue<float>
	{
		GenericRuntimeValue<float>   stat(25.5f, 50.0f, -10.0f);
		const auto                   bytes = StatsSRI::serialize_limited_value(stat);
		const std::vector<std::byte> buffer(bytes.begin(), bytes.end());

		const auto deserialized = StatsDSI::get_stats<float>(buffer);
		EXPECT_FLOAT_EQ(deserialized.value, stat.value());
		EXPECT_FLOAT_EQ(deserialized.max, stat.max());
		EXPECT_FLOAT_EQ(deserialized.min, stat.min());
	}

	// GenericRuntimeValue<uint32_t>
	{
		GenericRuntimeValue<std::uint32_t> stat(75U, 200U, 0U);
		const auto                         bytes = StatsSRI::serialize_limited_value(stat);
		const std::vector<std::byte>       buffer(bytes.begin(), bytes.end());

		const auto deserialized = StatsDSI::get_stats<std::uint32_t>(buffer);
		EXPECT_EQ(deserialized.value, stat.value());
		EXPECT_EQ(deserialized.max, stat.max());
		EXPECT_EQ(deserialized.min, stat.min());
	}

	// GenericRuntimeValue<uint64_t>
	{
		GenericRuntimeValue<std::uint64_t> stat(123456789ULL, 999999999ULL, 0ULL);
		const auto                         bytes = StatsSRI::serialize_limited_value(stat);
		const std::vector<std::byte>       buffer(bytes.begin(), bytes.end());

		const auto deserialized = StatsDSI::get_stats<std::uint64_t>(buffer);
		EXPECT_EQ(deserialized.value, stat.value());
		EXPECT_EQ(deserialized.max, stat.max());
		EXPECT_EQ(deserialized.min, stat.min());
	}

	// GenericRuntimeValue<uint8_t>
	{
		GenericRuntimeValue<std::uint8_t> stat(15, 100, 0);
		const auto                        bytes = StatsSRI::serialize_limited_value(stat);
		const std::vector<std::byte>      buffer(bytes.begin(), bytes.end());

		const auto deserialized = StatsDSI::get_stats<std::uint8_t>(buffer);
		EXPECT_EQ(deserialized.value, stat.value());
		EXPECT_EQ(deserialized.max, stat.max());
		EXPECT_EQ(deserialized.min, stat.min());
	}
}

// Math: Stats - Field Getters and Type

TEST(MathSerializeDeserializeRoundtrip, Stats_FieldGettersAndType) {
	// GenericRuntimeValue<double>
	{
		GenericRuntimeValue<double>  stat(50.0, 100.0, 0.0);
		const auto                   bytes = StatsSRI::serialize_limited_value(stat);
		const std::vector<std::byte> buffer(bytes.begin(), bytes.end());

		EXPECT_EQ(StatsDSI::get_type(buffer), BinaryNLT::DOUBLE);
		EXPECT_DOUBLE_EQ(StatsDSI::get_value<double>(buffer, StatsSRI::TO_GET_VALUE_OFFSET),
						 stat.value());
		EXPECT_DOUBLE_EQ(StatsDSI::get_max<double>(buffer), stat.max());
		EXPECT_DOUBLE_EQ(StatsDSI::get_min<double>(buffer), stat.min());
	}

	// GenericRuntimeValue<float>
	{
		GenericRuntimeValue<float>   stat(25.5f, 50.0f, -10.0f);
		const auto                   bytes = StatsSRI::serialize_limited_value(stat);
		const std::vector<std::byte> buffer(bytes.begin(), bytes.end());

		EXPECT_EQ(StatsDSI::get_type(buffer), BinaryNLT::FLOAT);
		EXPECT_FLOAT_EQ(StatsDSI::get_value<float>(buffer, StatsSRI::TO_GET_VALUE_OFFSET),
						stat.value());
		EXPECT_FLOAT_EQ(StatsDSI::get_max<float>(buffer), stat.max());
		EXPECT_FLOAT_EQ(StatsDSI::get_min<float>(buffer), stat.min());
	}

	// GenericRuntimeValue<uint32_t>
	{
		GenericRuntimeValue<std::uint32_t> stat(75U, 200U, 0U);
		const auto                         bytes = StatsSRI::serialize_limited_value(stat);
		const std::vector<std::byte>       buffer(bytes.begin(), bytes.end());

		EXPECT_EQ(StatsDSI::get_type(buffer), BinaryNLT::UINT32_T);
		EXPECT_EQ(StatsDSI::get_value<std::uint32_t>(buffer, StatsSRI::TO_GET_VALUE_OFFSET),
				  stat.value());
		EXPECT_EQ(StatsDSI::get_max<std::uint32_t>(buffer), stat.max());
		EXPECT_EQ(StatsDSI::get_min<std::uint32_t>(buffer), stat.min());
	}

	// GenericRuntimeValue<uint64_t>
	{
		GenericRuntimeValue<std::uint64_t> stat(123456789ULL, 999999999ULL, 0ULL);
		const auto                         bytes = StatsSRI::serialize_limited_value(stat);
		const std::vector<std::byte>       buffer(bytes.begin(), bytes.end());

		EXPECT_EQ(StatsDSI::get_type(buffer), BinaryNLT::UINT64_T);
		EXPECT_EQ(StatsDSI::get_value<std::uint64_t>(buffer, StatsSRI::TO_GET_VALUE_OFFSET),
				  stat.value());
		EXPECT_EQ(StatsDSI::get_max<std::uint64_t>(buffer), stat.max());
		EXPECT_EQ(StatsDSI::get_min<std::uint64_t>(buffer), stat.min());
	}

	// GenericRuntimeValue<uint8_t>
	{
		GenericRuntimeValue<std::uint8_t> stat(15, 100, 0);
		const auto                        bytes = StatsSRI::serialize_limited_value(stat);
		const std::vector<std::byte>      buffer(bytes.begin(), bytes.end());

		EXPECT_EQ(StatsDSI::get_type(buffer), BinaryNLT::UINT8_T);
		EXPECT_EQ(StatsDSI::get_value<std::uint8_t>(buffer, StatsSRI::TO_GET_VALUE_OFFSET),
				  stat.value());
		EXPECT_EQ(StatsDSI::get_max<std::uint8_t>(buffer), stat.max());
		EXPECT_EQ(StatsDSI::get_min<std::uint8_t>(buffer), stat.min());
	}
}

// Math: PhysicsUnits Roundtrip

TEST(MathSerializeDeserializeRoundtrip, PhysicsUnits) {
	// Volume
	{
		constexpr Volume             original{42.5};
		const auto                   bytes = PhysicsUnitsSRI::serialize_volume(original);
		const std::vector<std::byte> buffer(bytes.begin(), bytes.end());
		const auto                   result = PhysicsUnitsDSI::deserialize_volume(buffer);
		EXPECT_DOUBLE_EQ(result.value, original.value);
	}

	// Mass
	{
		const PhysicsStats::Mass     original{123.456};
		const auto                   bytes = PhysicsUnitsSRI::serialize_mass(original);
		const std::vector<std::byte> buffer(bytes.begin(), bytes.end());
		const auto                   result = PhysicsUnitsDSI::deserialize_mass(buffer);
		EXPECT_DOUBLE_EQ(result.value, original.value);
	}

	// Density
	{
		const PhysicsStats::Density  original{78.9};
		const auto                   bytes = PhysicsUnitsSRI::serialize_density(original);
		const std::vector<std::byte> buffer(bytes.begin(), bytes.end());
		const auto                   result = PhysicsUnitsDSI::deserialize_density(buffer);
		EXPECT_DOUBLE_EQ(result.value, original.value);
	}

	// Meter
	{
		const Meter                  original{9.81};
		const auto                   bytes = PhysicsUnitsSRI::serialize_meter(original);
		const std::vector<std::byte> buffer(bytes.begin(), bytes.end());
		const auto                   result = PhysicsUnitsDSI::deserialize_meter(buffer);
		EXPECT_DOUBLE_EQ(result.value, original.value);
	}

	// Lateral, Height, Depth
	{
		const Lateral original_lat{1.2};
		const Height  original_h{3.4};
		const Depth   original_d{5.6};

		const auto bytes_lat = PhysicsUnitsSRI::serialize_lateral(original_lat);
		const auto bytes_h   = PhysicsUnitsSRI::serialize_height(original_h);
		const auto bytes_d   = PhysicsUnitsSRI::serialize_depth(original_d);

		const std::vector<std::byte> buffer_lat(bytes_lat.begin(), bytes_lat.end());
		const std::vector<std::byte> buffer_h(bytes_h.begin(), bytes_h.end());
		const std::vector<std::byte> buffer_d(bytes_d.begin(), bytes_d.end());

		EXPECT_DOUBLE_EQ(PhysicsUnitsDSI::deserialize_lateral(buffer_lat).value,
						 original_lat.value);
		EXPECT_DOUBLE_EQ(PhysicsUnitsDSI::deserialize_height(buffer_h).value, original_h.value);
		EXPECT_DOUBLE_EQ(PhysicsUnitsDSI::deserialize_depth(buffer_d).value, original_d.value);
	}

	// Size
	{
		constexpr Size               original{.lateral = Lateral{2.5},
											  .height  = Height{4.0},
											  .depth   = Depth{1.5}};
		const auto                   bytes = PhysicsUnitsSRI::serialize_size(original);
		const std::vector<std::byte> buffer(bytes.begin(), bytes.end());
		const auto                   result = PhysicsUnitsDSI::deserialize_size(buffer);
		EXPECT_DOUBLE_EQ(result.lateral.value, original.lateral.value);
		EXPECT_DOUBLE_EQ(result.height.value, original.height.value);
		EXPECT_DOUBLE_EQ(result.depth.value, original.depth.value);
	}

	// SharedVolume
	{
		const PhysicsStats::SharedVolume original{0.75};
		const auto                       bytes = PhysicsUnitsSRI::serialize_shared_volume(original);
		const std::vector<std::byte>     buffer(bytes.begin(), bytes.end());
		const auto result = PhysicsUnitsDSI::deserialize_shared_volume(buffer);
		EXPECT_DOUBLE_EQ(result.value(), original.value());
	}

	// Efficiency
	{
		const Efficiency             original{0.85f};
		const auto                   bytes = PhysicsUnitsSRI::serialize_efficiency(original);
		const std::vector<std::byte> buffer(bytes.begin(), bytes.end());
		const auto                   result = PhysicsUnitsDSI::deserialize_efficiency(buffer);
		EXPECT_FLOAT_EQ(result.value(), original.value());
	}

	// Quality
	{
		const Quality                original{0.92f};
		const auto                   bytes = PhysicsUnitsSRI::serialize_quality(original);
		const std::vector<std::byte> buffer(bytes.begin(), bytes.end());
		const auto                   result = PhysicsUnitsDSI::deserialize_quality(buffer);
		EXPECT_FLOAT_EQ(result.value(), original.value());
	}
}

// Math: Stats - NormalizedValue Roundtrip (serialize_normalized_value)

TEST(MathSerializeDeserializeRoundtrip, Stats_NormalizedValueRoundtrip) {
	// NormalizedValue<double>
	{
		for (double val : {0.0, 0.25, 0.5, 0.75, 1.0}) {
			NormalizedValue<double>      original(val);
			const auto                   bytes = StatsSRI::serialize_normalized_value(original);
			const std::vector<std::byte> buffer(bytes.begin(), bytes.end());

			EXPECT_EQ(StatsDSI::get_type(buffer), BinaryNLT::DOUBLE);
			EXPECT_DOUBLE_EQ(StatsDSI::get_value<double>(buffer, StatsSRI::TO_GET_VALUE_OFFSET),
							 original.value());
			EXPECT_DOUBLE_EQ(StatsDSI::get_max<double>(buffer), 1.0);
			EXPECT_DOUBLE_EQ(StatsDSI::get_min<double>(buffer), 0.0);

			const auto stats = StatsDSI::get_stats<double>(buffer);
			EXPECT_DOUBLE_EQ(stats.value, original.value());
			EXPECT_DOUBLE_EQ(stats.max, 1.0);
			EXPECT_DOUBLE_EQ(stats.min, 0.0);
		}

		// Clamping behavior
		{
			NormalizedValue<double> clamped_high(5.0);
			const auto              bytes_h = StatsSRI::serialize_normalized_value(clamped_high);
			const std::vector<std::byte> buffer_h(bytes_h.begin(), bytes_h.end());
			EXPECT_DOUBLE_EQ(StatsDSI::get_value<double>(buffer_h, StatsSRI::TO_GET_VALUE_OFFSET),
							 1.0);

			NormalizedValue<double> clamped_low(-3.0);
			const auto              bytes_l = StatsSRI::serialize_normalized_value(clamped_low);
			const std::vector<std::byte> buffer_l(bytes_l.begin(), bytes_l.end());
			EXPECT_DOUBLE_EQ(StatsDSI::get_value<double>(buffer_l, StatsSRI::TO_GET_VALUE_OFFSET),
							 0.0);
		}
	}

	// NormalizedValue<float>
	{
		for (float val : {0.0f, 0.33f, 0.66f, 1.0f}) {
			NormalizedValue<float>       original(val);
			const auto                   bytes = StatsSRI::serialize_normalized_value(original);
			const std::vector<std::byte> buffer(bytes.begin(), bytes.end());

			EXPECT_EQ(StatsDSI::get_type(buffer), BinaryNLT::FLOAT);
			EXPECT_FLOAT_EQ(StatsDSI::get_value<float>(buffer, StatsSRI::TO_GET_VALUE_OFFSET),
							original.value());
			EXPECT_FLOAT_EQ(StatsDSI::get_max<float>(buffer), 1.0f);
			EXPECT_FLOAT_EQ(StatsDSI::get_min<float>(buffer), 0.0f);

			const auto stats = StatsDSI::get_stats<float>(buffer);
			EXPECT_FLOAT_EQ(stats.value, original.value());
			EXPECT_FLOAT_EQ(stats.max, 1.0f);
			EXPECT_FLOAT_EQ(stats.min, 0.0f);
		}
	}
}

// Math: Stats - SignedNormalizedValue Roundtrip (serialize_signed_normalized_value)

TEST(MathSerializeDeserializeRoundtrip, Stats_SignedNormalizedValueRoundtrip) {
	// SignedNormalizedValue<double>
	{
		for (double val : {-1.0, -0.5, 0.0, 0.5, 1.0}) {
			SignedNormalizedValue<double> original(val);
			const auto bytes = StatsSRI::serialize_signed_normalized_value(original);
			const std::vector<std::byte> buffer(bytes.begin(), bytes.end());

			EXPECT_EQ(StatsDSI::get_type(buffer), BinaryNLT::DOUBLE);
			EXPECT_DOUBLE_EQ(StatsDSI::get_value<double>(buffer, StatsSRI::TO_GET_VALUE_OFFSET),
							 original.value());
			EXPECT_DOUBLE_EQ(StatsDSI::get_max<double>(buffer), 1.0);
			EXPECT_DOUBLE_EQ(StatsDSI::get_min<double>(buffer), -1.0);

			const auto stats = StatsDSI::get_stats<double>(buffer);
			EXPECT_DOUBLE_EQ(stats.value, original.value());
			EXPECT_DOUBLE_EQ(stats.max, 1.0);
			EXPECT_DOUBLE_EQ(stats.min, -1.0);
		}

		// Clamping behavior
		{
			SignedNormalizedValue<double> clamped_high(10.0);
			const auto bytes_h = StatsSRI::serialize_signed_normalized_value(clamped_high);
			const std::vector<std::byte> buffer_h(bytes_h.begin(), bytes_h.end());
			EXPECT_DOUBLE_EQ(StatsDSI::get_value<double>(buffer_h, StatsSRI::TO_GET_VALUE_OFFSET),
							 1.0);

			SignedNormalizedValue<double> clamped_low(-10.0);
			const auto bytes_l = StatsSRI::serialize_signed_normalized_value(clamped_low);
			const std::vector<std::byte> buffer_l(bytes_l.begin(), bytes_l.end());
			EXPECT_DOUBLE_EQ(StatsDSI::get_value<double>(buffer_l, StatsSRI::TO_GET_VALUE_OFFSET),
							 -1.0);
		}
	}

	// SignedNormalizedValue<float>
	{
		for (float val : {-1.0f, -0.5f, 0.0f, 0.5f, 1.0f}) {
			SignedNormalizedValue<float> original(val);
			const auto bytes = StatsSRI::serialize_signed_normalized_value(original);
			const std::vector<std::byte> buffer(bytes.begin(), bytes.end());

			EXPECT_EQ(StatsDSI::get_type(buffer), BinaryNLT::FLOAT);
			EXPECT_FLOAT_EQ(StatsDSI::get_value<float>(buffer, StatsSRI::TO_GET_VALUE_OFFSET),
							original.value());
			EXPECT_FLOAT_EQ(StatsDSI::get_max<float>(buffer), 1.0f);
			EXPECT_FLOAT_EQ(StatsDSI::get_min<float>(buffer), -1.0f);

			const auto stats = StatsDSI::get_stats<float>(buffer);
			EXPECT_FLOAT_EQ(stats.value, original.value());
			EXPECT_FLOAT_EQ(stats.max, 1.0f);
			EXPECT_FLOAT_EQ(stats.min, -1.0f);
		}
	}
}

// Math: PhysicsUnits - Randomized Multi-iteration Roundtrip

TEST(MathSerializeDeserializeRoundtrip, PhysicsUnits_RandomizedRoundtrip) {
	for (int iteration = 0; iteration < 10; ++iteration) {
		SCOPED_TRACE(::testing::Message() << "iteration " << iteration);

		const double rand_val1 = RandomGenerators::generate<double>();
		const double rand_val2 = RandomGenerators::generate<double>();
		const double rand_val3 = RandomGenerators::generate<double>();
		const double rand_val4 = RandomGenerators::generate<double>();

		// Volume
		{
			const Volume                 original{rand_val1};
			const auto                   bytes = PhysicsUnitsSRI::serialize_volume(original);
			const std::vector<std::byte> buffer(bytes.begin(), bytes.end());
			EXPECT_DOUBLE_EQ(PhysicsUnitsDSI::deserialize_volume(buffer).value, original.value);
		}

		// Mass
		{
			const PhysicsStats::Mass     original{rand_val2};
			const auto                   bytes = PhysicsUnitsSRI::serialize_mass(original);
			const std::vector<std::byte> buffer(bytes.begin(), bytes.end());
			EXPECT_DOUBLE_EQ(PhysicsUnitsDSI::deserialize_mass(buffer).value, original.value);
		}

		// Density
		{
			const PhysicsStats::Density  original{rand_val3};
			const auto                   bytes = PhysicsUnitsSRI::serialize_density(original);
			const std::vector<std::byte> buffer(bytes.begin(), bytes.end());
			EXPECT_DOUBLE_EQ(PhysicsUnitsDSI::deserialize_density(buffer).value, original.value);
		}

		// Meter, Lateral, Height, Depth
		{
			const Meter   orig_meter{rand_val4};
			const Lateral orig_lat{rand_val1};
			const Height  orig_h{rand_val2};
			const Depth   orig_d{rand_val3};

			const auto b_m   = PhysicsUnitsSRI::serialize_meter(orig_meter);
			const auto b_lat = PhysicsUnitsSRI::serialize_lateral(orig_lat);
			const auto b_h   = PhysicsUnitsSRI::serialize_height(orig_h);
			const auto b_d   = PhysicsUnitsSRI::serialize_depth(orig_d);

			EXPECT_DOUBLE_EQ(
				PhysicsUnitsDSI::deserialize_meter(std::vector<std::byte>(b_m.begin(), b_m.end()))
					.value,
				orig_meter.value);
			EXPECT_DOUBLE_EQ(PhysicsUnitsDSI::deserialize_lateral(
								 std::vector<std::byte>(b_lat.begin(), b_lat.end()))
								 .value,
							 orig_lat.value);
			EXPECT_DOUBLE_EQ(
				PhysicsUnitsDSI::deserialize_height(std::vector<std::byte>(b_h.begin(), b_h.end()))
					.value,
				orig_h.value);
			EXPECT_DOUBLE_EQ(
				PhysicsUnitsDSI::deserialize_depth(std::vector<std::byte>(b_d.begin(), b_d.end()))
					.value,
				orig_d.value);
		}

		// Size
		{
			const Size                   original{.lateral = Lateral{rand_val1},
												  .height  = Height{rand_val2},
												  .depth   = Depth{rand_val3}};
			const auto                   bytes = PhysicsUnitsSRI::serialize_size(original);
			const std::vector<std::byte> buffer(bytes.begin(), bytes.end());
			const auto                   result = PhysicsUnitsDSI::deserialize_size(buffer);
			EXPECT_DOUBLE_EQ(result.lateral.value, original.lateral.value);
			EXPECT_DOUBLE_EQ(result.height.value, original.height.value);
			EXPECT_DOUBLE_EQ(result.depth.value, original.depth.value);
		}
	}
}

// Math: PhysicsUnits - Stream With Offsets

TEST(MathSerializeDeserializeRoundtrip, PhysicsUnits_StreamWithOffsets) {
	const Volume                orig_vol{10.5};
	const PhysicsStats::Mass    orig_mass{200.0};
	const PhysicsStats::Density orig_dens{1.25};
	const Meter                 orig_meter{55.0};
	const Size orig_size{.lateral = Lateral{2.0}, .height = Height{3.0}, .depth = Depth{4.0}};
	const PhysicsStats::SharedVolume orig_shared{0.6};
	const Efficiency                 orig_eff{0.8f};
	const Quality                    orig_qual{0.9f};

	std::vector<std::byte> stream = {std::byte{0xDE}, std::byte{0xAD}};

	const size_t off_vol = stream.size();
	append_to_buffer(stream, PhysicsUnitsSRI::serialize_volume(orig_vol));

	const size_t off_mass = stream.size();
	append_to_buffer(stream, PhysicsUnitsSRI::serialize_mass(orig_mass));

	const size_t off_dens = stream.size();
	append_to_buffer(stream, PhysicsUnitsSRI::serialize_density(orig_dens));

	const size_t off_meter = stream.size();
	append_to_buffer(stream, PhysicsUnitsSRI::serialize_meter(orig_meter));

	const size_t off_size = stream.size();
	append_to_buffer(stream, PhysicsUnitsSRI::serialize_size(orig_size));

	const size_t off_shared = stream.size();
	append_to_buffer(stream, PhysicsUnitsSRI::serialize_shared_volume(orig_shared));

	const size_t off_eff = stream.size();
	append_to_buffer(stream, PhysicsUnitsSRI::serialize_efficiency(orig_eff));

	const size_t off_qual = stream.size();
	append_to_buffer(stream, PhysicsUnitsSRI::serialize_quality(orig_qual));

	stream.push_back(std::byte{0xBE});
	stream.push_back(std::byte{0xEF});

	EXPECT_DOUBLE_EQ(PhysicsUnitsDSI::deserialize_volume(stream, off_vol).value, orig_vol.value);
	EXPECT_DOUBLE_EQ(PhysicsUnitsDSI::deserialize_mass(stream, off_mass).value, orig_mass.value);
	EXPECT_DOUBLE_EQ(PhysicsUnitsDSI::deserialize_density(stream, off_dens).value, orig_dens.value);
	EXPECT_DOUBLE_EQ(PhysicsUnitsDSI::deserialize_meter(stream, off_meter).value, orig_meter.value);

	const auto deserialized_size = PhysicsUnitsDSI::deserialize_size(stream, off_size);
	EXPECT_DOUBLE_EQ(deserialized_size.lateral.value, orig_size.lateral.value);
	EXPECT_DOUBLE_EQ(deserialized_size.height.value, orig_size.height.value);
	EXPECT_DOUBLE_EQ(deserialized_size.depth.value, orig_size.depth.value);

	EXPECT_DOUBLE_EQ(PhysicsUnitsDSI::deserialize_shared_volume(stream, off_shared).value(),
					 orig_shared.value());
	EXPECT_FLOAT_EQ(PhysicsUnitsDSI::deserialize_efficiency(stream, off_eff).value(),
					orig_eff.value());
	EXPECT_FLOAT_EQ(PhysicsUnitsDSI::deserialize_quality(stream, off_qual).value(),
					orig_qual.value());
}

// Math: PhysicsUnits - Size Operations and Volume Calculation

TEST(MathSerializeDeserializeRoundtrip, PhysicsUnits_SizeOperations) {
	const Size size{.lateral = Lateral{2.0}, .height = Height{3.0}, .depth = Depth{4.0}};

	const auto                   bytes = PhysicsUnitsSRI::serialize_size(size);
	const std::vector<std::byte> buffer(bytes.begin(), bytes.end());
	const auto                   deserialized = PhysicsUnitsDSI::deserialize_size(buffer);

	EXPECT_DOUBLE_EQ(deserialized.volume().value, 24.0);
	EXPECT_DOUBLE_EQ(deserialized.volume().value, size.volume().value);

	const auto static_vol =
		Size::volume(deserialized.lateral, deserialized.height, deserialized.depth);
	EXPECT_DOUBLE_EQ(static_vol.value, 24.0);

	Size modified = deserialized;
	modified.modify(NormalizedValue<float>(0.5f));
	EXPECT_DOUBLE_EQ(modified.lateral.value, 1.0);
	EXPECT_DOUBLE_EQ(modified.height.value, 1.5);
	EXPECT_DOUBLE_EQ(modified.depth.value, 2.0);
	EXPECT_DOUBLE_EQ(modified.volume().value, 3.0);

	const auto                   mod_bytes = PhysicsUnitsSRI::serialize_size(modified);
	const std::vector<std::byte> mod_buffer(mod_bytes.begin(), mod_bytes.end());
	const auto                   mod_deserialized = PhysicsUnitsDSI::deserialize_size(mod_buffer);
	EXPECT_DOUBLE_EQ(mod_deserialized.volume().value, 3.0);
}

// Math: PhysicsUnits - Insufficient Buffer Size Throws Error

TEST(MathSerializeDeserializeRoundtrip, PhysicsUnits_InsufficientBufferSizeThrowsError) {
	const std::vector<std::byte> empty_buffer;
	const std::vector<std::byte> short_buffer = {std::byte{0x01}, std::byte{0x02}, std::byte{0x03}};

	EXPECT_THROW(PhysicsUnitsDSI::deserialize_volume(empty_buffer), ByteError);
	EXPECT_THROW(PhysicsUnitsDSI::deserialize_volume(short_buffer), ByteError);
	EXPECT_THROW(PhysicsUnitsDSI::deserialize_mass(short_buffer), ByteError);
	EXPECT_THROW(PhysicsUnitsDSI::deserialize_density(short_buffer), ByteError);
	EXPECT_THROW(PhysicsUnitsDSI::deserialize_meter(short_buffer), ByteError);
	EXPECT_THROW(PhysicsUnitsDSI::deserialize_lateral(short_buffer), ByteError);
	EXPECT_THROW(PhysicsUnitsDSI::deserialize_height(short_buffer), ByteError);
	EXPECT_THROW(PhysicsUnitsDSI::deserialize_depth(short_buffer), ByteError);

	std::vector<std::byte> size_short_buffer(16, std::byte{0x01});
	EXPECT_THROW(PhysicsUnitsDSI::deserialize_size(size_short_buffer), ByteError);

	// shared_volume reads double (8 bytes) at TO_GET_VALUE_OFFSET=1 -> needs 9 bytes minimum
	std::vector<std::byte> shared_vol_short_buffer(8, std::byte{0x01});
	EXPECT_THROW(PhysicsUnitsDSI::deserialize_shared_volume(shared_vol_short_buffer), ByteError);

	// efficiency/quality read float (4 bytes) at TO_GET_VALUE_OFFSET=1 -> needs 5 bytes minimum
	std::vector<std::byte> stat_float_short_buffer(4, std::byte{0x01});
	EXPECT_THROW(PhysicsUnitsDSI::deserialize_efficiency(stat_float_short_buffer), ByteError);
	EXPECT_THROW(PhysicsUnitsDSI::deserialize_quality(stat_float_short_buffer), ByteError);
}

// Math: Geometry Roundtrip

TEST(MathSerializeDeserializeRoundtrip, Geometry_Roundtrip) {
	// Radius and its computed properties
	{
		const GeometryForms::Radius  original{5.0};
		const auto                   bytes = GeometrySRI::serialize_radius(original);
		const std::vector<std::byte> buffer(bytes.begin(), bytes.end());
		const auto                   result = GeometryDSI::deserialize_radius(buffer);

		EXPECT_DOUBLE_EQ(result.value, original.value);
	}

	// Circumference
	{
		constexpr GeometryForms::Circumference original{31.41592653589793};
		const auto                   bytes = GeometrySRI::serialize_circumference(original);
		const std::vector<std::byte> buffer(bytes.begin(), bytes.end());
		const auto                   result = GeometryDSI::deserialize_circumference(buffer);

		EXPECT_DOUBLE_EQ(result.value, original.value);
	}

	// Diameter
	{
		constexpr GeometryForms::Diameter original{10.0};
		const auto                        bytes = GeometrySRI::serialize_diameter(original);
		const std::vector<std::byte>      buffer(bytes.begin(), bytes.end());
		const auto                        result = GeometryDSI::deserialize_diameter(buffer);

		EXPECT_DOUBLE_EQ(result.value, original.value);
	}

	// Area
	{
		constexpr GeometryForms::Area original{78.53981633974483};
		const auto                    bytes = GeometrySRI::serialize_area(original);
		const std::vector<std::byte>  buffer(bytes.begin(), bytes.end());
		const auto                    result = GeometryDSI::deserialize_area(buffer);

		EXPECT_DOUBLE_EQ(result.value, original.value);
	}
}

// Math: Geometry - Randomized Multi-iteration Roundtrip

TEST(MathSerializeDeserializeRoundtrip, Geometry_RandomizedRoundtrip) {
	for (int iteration = 0; iteration < 10; ++iteration) {
		SCOPED_TRACE(::testing::Message() << "iteration " << iteration);

		const double rand_r = RandomGenerators::generate<double>();
		const double rand_c = RandomGenerators::generate<double>();
		const double rand_d = RandomGenerators::generate<double>();
		const double rand_a = RandomGenerators::generate<double>();

		// Radius
		{
			const GeometryForms::Radius  orig{rand_r};
			const auto                   bytes = GeometrySRI::serialize_radius(orig);
			const std::vector<std::byte> buffer(bytes.begin(), bytes.end());
			const auto                   res = GeometryDSI::deserialize_radius(buffer);
			EXPECT_DOUBLE_EQ(res.value, orig.value);
		}

		// Circumference
		{
			const GeometryForms::Circumference orig{rand_c};
			const auto                         bytes = GeometrySRI::serialize_circumference(orig);
			const std::vector<std::byte>       buffer(bytes.begin(), bytes.end());
			EXPECT_DOUBLE_EQ(GeometryDSI::deserialize_circumference(buffer).value, orig.value);
		}

		// Diameter
		{
			const GeometryForms::Diameter orig{rand_d};
			const auto                    bytes = GeometrySRI::serialize_diameter(orig);
			const std::vector<std::byte>  buffer(bytes.begin(), bytes.end());
			EXPECT_DOUBLE_EQ(GeometryDSI::deserialize_diameter(buffer).value, orig.value);
		}

		// Area
		{
			const GeometryForms::Area    orig{rand_a};
			const auto                   bytes = GeometrySRI::serialize_area(orig);
			const std::vector<std::byte> buffer(bytes.begin(), bytes.end());
			EXPECT_DOUBLE_EQ(GeometryDSI::deserialize_area(buffer).value, orig.value);
		}
	}
}

// Math: Geometry - Stream With Offsets

TEST(MathSerializeDeserializeRoundtrip, Geometry_StreamWithOffsets) {
	constexpr GeometryForms::Radius        orig_radius{7.5};
	constexpr GeometryForms::Circumference orig_circ{47.12388980384689};
	constexpr GeometryForms::Diameter      orig_diam{15.0};
	constexpr GeometryForms::Area          orig_area{176.71458676442586};

	std::vector<std::byte> stream = {std::byte{0xAA}, std::byte{0xBB}, std::byte{0xCC}};

	const size_t off_r = stream.size();
	append_to_buffer(stream, GeometrySRI::serialize_radius(orig_radius));

	const size_t off_c = stream.size();
	append_to_buffer(stream, GeometrySRI::serialize_circumference(orig_circ));

	const size_t off_d = stream.size();
	append_to_buffer(stream, GeometrySRI::serialize_diameter(orig_diam));

	const size_t off_a = stream.size();
	append_to_buffer(stream, GeometrySRI::serialize_area(orig_area));

	stream.push_back(std::byte{0xDD});
	stream.push_back(std::byte{0xEE});

	EXPECT_DOUBLE_EQ(GeometryDSI::deserialize_radius(stream, off_r).value, orig_radius.value);
	EXPECT_DOUBLE_EQ(GeometryDSI::deserialize_circumference(stream, off_c).value, orig_circ.value);
	EXPECT_DOUBLE_EQ(GeometryDSI::deserialize_diameter(stream, off_d).value, orig_diam.value);
	EXPECT_DOUBLE_EQ(GeometryDSI::deserialize_area(stream, off_a).value, orig_area.value);
}

// Math: Geometry - Insufficient Buffer Size Throws Error

TEST(MathSerializeDeserializeRoundtrip, Geometry_InsufficientBufferSizeThrowsError) {
	const std::vector<std::byte> empty_buffer;
	const std::vector<std::byte> short_buffer = {std::byte{0x01}, std::byte{0x02}, std::byte{0x03}};

	EXPECT_THROW(GeometryDSI::deserialize_radius(empty_buffer), ByteError);
	EXPECT_THROW(GeometryDSI::deserialize_radius(short_buffer), ByteError);
	EXPECT_THROW(GeometryDSI::deserialize_circumference(empty_buffer), ByteError);
	EXPECT_THROW(GeometryDSI::deserialize_circumference(short_buffer), ByteError);
	EXPECT_THROW(GeometryDSI::deserialize_diameter(empty_buffer), ByteError);
	EXPECT_THROW(GeometryDSI::deserialize_diameter(short_buffer), ByteError);
	EXPECT_THROW(GeometryDSI::deserialize_area(empty_buffer), ByteError);
	EXPECT_THROW(GeometryDSI::deserialize_area(short_buffer), ByteError);

	const std::vector<std::byte> valid_size_buffer(sizeof(double), std::byte{0x01});
	EXPECT_THROW(GeometryDSI::deserialize_radius(valid_size_buffer, 1), ByteError);
	EXPECT_THROW(GeometryDSI::deserialize_circumference(valid_size_buffer, 1), ByteError);
	EXPECT_THROW(GeometryDSI::deserialize_diameter(valid_size_buffer, 1), ByteError);
	EXPECT_THROW(GeometryDSI::deserialize_area(valid_size_buffer, 1), ByteError);
}

// ByteUtils: Zero Padding

TEST(ByteUtilsRoundtrip, ZeroPadding) {
	std::array<std::byte, 5> array = {std::byte{0x01},
									  std::byte{0x02},
									  std::byte{0x03},
									  std::byte{0x04},
									  std::byte{0x05}};

	ByteUtils::zero_padding(array, 2);
	EXPECT_EQ(array[0], std::byte{0x01});
	EXPECT_EQ(array[1], std::byte{0x02});
	EXPECT_EQ(array[2], std::byte{0x00});
	EXPECT_EQ(array[3], std::byte{0x00});
	EXPECT_EQ(array[4], std::byte{0x00});

	ByteUtils::zero_padding(array, 0);
	for (const auto &byte : array) {
		EXPECT_EQ(byte, std::byte{0x00});
	}

	std::vector<std::byte> vec = {std::byte{0xAA}, std::byte{0xBB}};
	ByteUtils::zero_padding(vec, 2);
	EXPECT_EQ(vec[0], std::byte{0xAA});
	EXPECT_EQ(vec[1], std::byte{0xBB});

	EXPECT_THROW(ByteUtils::zero_padding(vec, 3), ByteError);
}

// Properties: Capabilities Move Roundtrip

TEST(PropertiesSerializeDeserializeRoundtrip, CapabilitiesMove_Roundtrip) {
	{
		const auto move = Properties::Capabilities::Move::SWIMM;
		const auto byte = PropertiesSRI::serialize(move);
		EXPECT_EQ(byte, std::byte{0x01});

		const auto deserialized = PropertiesDSI::deserialize(byte);
		EXPECT_TRUE(std::holds_alternative<Properties::Capabilities::Move>(deserialized));
		EXPECT_EQ(std::get<Properties::Capabilities::Move>(deserialized), move);
	}

	{
		const auto move = Properties::Capabilities::Move::WALK;
		const auto byte = PropertiesSRI::serialize(move);
		EXPECT_EQ(byte, std::byte{0x02});

		const auto deserialized = PropertiesDSI::deserialize(byte);
		EXPECT_TRUE(std::holds_alternative<Properties::Capabilities::Move>(deserialized));
		EXPECT_EQ(std::get<Properties::Capabilities::Move>(deserialized), move);
	}
}

// Properties: GenericProperty Roundtrip

TEST(PropertiesSerializeDeserializeRoundtrip, GenericProperty_Roundtrip) {
	const GenericProperty prop_swimm = Properties::Capabilities::Move::SWIMM;
	const auto            byte_swimm = PropertiesSRI::serialize(prop_swimm);
	EXPECT_EQ(byte_swimm, std::byte{0x01});
	EXPECT_EQ(PropertiesDSI::deserialize(byte_swimm), prop_swimm);

	const GenericProperty prop_walk = Properties::Capabilities::Move::WALK;
	const auto            byte_walk = PropertiesSRI::serialize(prop_walk);
	EXPECT_EQ(byte_walk, std::byte{0x02});
	EXPECT_EQ(PropertiesDSI::deserialize(byte_walk), prop_walk);
}

// Properties: Stream With Offsets

TEST(PropertiesSerializeDeserializeRoundtrip, StreamWithOffsets) {
	std::vector<std::byte> stream = {std::byte{0xAA}, std::byte{0xBB}};

	const size_t offset_swimm = stream.size();
	append_to_buffer(stream, PropertiesSRI::serialize(Properties::Capabilities::Move::SWIMM));

	const size_t offset_walk = stream.size();
	append_to_buffer(stream, PropertiesSRI::serialize(Properties::Capabilities::Move::WALK));

	stream.push_back(std::byte{0xCC});

	const auto deserialized_swimm = PropertiesDSI::deserialize(stream, offset_swimm);
	const auto deserialized_walk  = PropertiesDSI::deserialize(stream, offset_walk);

	EXPECT_EQ(std::get<Properties::Capabilities::Move>(deserialized_swimm),
			  Properties::Capabilities::Move::SWIMM);
	EXPECT_EQ(std::get<Properties::Capabilities::Move>(deserialized_walk),
			  Properties::Capabilities::Move::WALK);
}

// Properties: Error Handling

TEST(PropertiesSerializeDeserializeRoundtrip, ErrorHandling) {
	EXPECT_THROW(PropertiesDSI::deserialize(std::byte{0x00}), ByteError);
	EXPECT_THROW(PropertiesDSI::deserialize(std::byte{0x03}), ByteError);
	EXPECT_THROW(PropertiesDSI::deserialize(std::byte{0xFF}), ByteError);

	const std::array<std::byte, 1> invalid_stream = {std::byte{0x99}};
	EXPECT_THROW(PropertiesDSI::deserialize(invalid_stream, 0), ByteError);
}

// Terrain Soil: Types and Properties Serialization

TEST(SoilSerializeDeserializeRoundtrip, TypesAndPropertiesSerialization) {
	EXPECT_EQ(SoilSRI::serialize_type(SoilTypes::DIRT), std::byte{0x01});
	EXPECT_EQ(SoilSRI::serialize_type(SoilTypes::ROCK), std::byte{0x02});
	EXPECT_EQ(SoilSRI::serialize_type(SoilTypes::SAND), std::byte{0x03});
	EXPECT_EQ(SoilSRI::serialize_type(SoilTypes::WATER), std::byte{0x04});
	EXPECT_THROW(SoilSRI::serialize_type(static_cast<SoilTypes>(99)), ByteError);

	EXPECT_EQ(SoilSRI::serialize_property(SoilProperties::DANGEROUS), std::byte{0x01});
	EXPECT_THROW(SoilSRI::serialize_property(static_cast<SoilProperties>(99)), ByteError);

	const std::vector<SoilProperties> valid_props = {SoilProperties::DANGEROUS};
	const auto                        props_bytes = SoilSRI::serialize_properties(valid_props);
	EXPECT_EQ(props_bytes[0], std::byte{0x01});

	std::vector<SoilProperties> oversized_props(SoilSRI::PROPERTIES_BYTE + 1,
												SoilProperties::DANGEROUS);
	EXPECT_THROW(SoilSRI::serialize_properties(oversized_props), ByteError);
}

// Terrain Soil: Required Capabilities Roundtrip

TEST(SoilSerializeDeserializeRoundtrip, RequiredCapabilities_Roundtrip) {
	const std::vector<GenericProperty> capabilities = {Properties::Capabilities::Move::WALK,
													   Properties::Capabilities::Move::SWIMM};

	const auto bytes = SoilSRI::serialize_required_capabilities(capabilities);

	const auto deserialized_0 =
		PropertiesDSI::deserialize(bytes, 0 * PropertiesSRI::PROPERTY_BYTES);
	const auto deserialized_1 =
		PropertiesDSI::deserialize(bytes, 1 * PropertiesSRI::PROPERTY_BYTES);

	EXPECT_EQ(deserialized_0, capabilities[0]);
	EXPECT_EQ(deserialized_1, capabilities[1]);

	std::vector<GenericProperty> oversized_capabilities(SoilSRI::REQUIRED_CAPABILITIES_BYTES + 1,
														Properties::Capabilities::Move::WALK);
	EXPECT_THROW(SoilSRI::serialize_required_capabilities(oversized_capabilities), ByteError);
}

// Terrain Soil: Components and ID Roundtrip

TEST(SoilSerializeDeserializeRoundtrip, ComponentsAndId) {
	SoilPiece soil_with_components{.type                  = SoilTypes::DIRT,
								   .properties            = {SoilProperties::DANGEROUS},
								   .required_capabilities = {Properties::Capabilities::Move::WALK},
								   .components            = {},
								   .radius                = GeometryForms::Radius{10.0},
								   .position              = Vec3{1.0, 2.0, 3.0},
								   .id                    = 987654321ULL};

	soil_with_components.components.add(SoilPieceComponents::Damage(25.5f));
	soil_with_components.components.add(SoilPieceComponents::MovementCost(1.75f));

	const auto damage_bytes = SoilSRI::serialize_damage(soil_with_components);
	EXPECT_FLOAT_EQ(Deserializer::read_float(damage_bytes, 1), 25.5f);

	const auto movement_cost_bytes = SoilSRI::serialize_movement_cost(soil_with_components);
	EXPECT_FLOAT_EQ(Deserializer::read_float(movement_cost_bytes, 1), 1.75f);

	const auto id_bytes = SoilSRI::serialize_id(soil_with_components);
	EXPECT_EQ(Deserializer::read_uint64_t(id_bytes, 0), 987654321ULL);

	SoilPiece empty_soil{.type                  = SoilTypes::SAND,
						 .properties            = {},
						 .required_capabilities = {},
						 .components            = {},
						 .radius                = GeometryForms::Radius{1.0},
						 .position              = Vec3{0.0, 0.0, 0.0},
						 .id                    = 0ULL};

	const auto empty_damage = SoilSRI::serialize_damage(empty_soil);
	for (const auto &byte : empty_damage) {
		EXPECT_EQ(byte, std::byte{0x00});
	}

	const auto empty_cost = SoilSRI::serialize_movement_cost(empty_soil);
	for (const auto &byte : empty_cost) {
		EXPECT_EQ(byte, std::byte{0x00});
	}
}

// Terrain Soil: Full SoilPiece Serialization Roundtrip

TEST(SoilSerializeDeserializeRoundtrip, FullSoilPiece_SerializationRoundtrip) {
	SoilPiece soil{.type                  = SoilTypes::ROCK,
				   .properties            = {SoilProperties::DANGEROUS},
				   .required_capabilities = {Properties::Capabilities::Move::SWIMM},
				   .components            = {},
				   .radius                = GeometryForms::Radius{12.5},
				   .position              = Vec3{4.0, 5.0, 6.0},
				   .id                    = 1122334455667788ULL};

	soil.components.add(SoilPieceComponents::Damage(50.0f));
	soil.components.add(SoilPieceComponents::MovementCost(3.0f));

	const auto bytes = SoilSRI::serialize_soil(soil);

	EXPECT_EQ(bytes[SoilSRI::TO_GET_TYPE_OFFSET], SoilSRI::serialize_type(SoilTypes::ROCK));
	EXPECT_EQ(bytes[SoilSRI::TO_GET_PROPERTIES_OFFSET], std::byte{0x01});

	const auto deserialized_cap =
		PropertiesDSI::deserialize(bytes, SoilSRI::TO_GET_REQUIRED_CAPABILITIES_OFFSET);
	EXPECT_EQ(deserialized_cap, GenericProperty(Properties::Capabilities::Move::SWIMM));

	const std::vector<std::byte> buffer(bytes.begin(), bytes.end());
	const auto deserialized_pos = VecDSI::deserialize_vec(buffer, SoilSRI::TO_GET_POSITION_OFFSET);
	EXPECT_EQ(deserialized_pos, (Vec3{4.0, 5.0, 6.0}));

	const auto deserialized_id = Deserializer::read_uint64_t(bytes, SoilSRI::TO_GET_ID_OFFSET);
	EXPECT_EQ(deserialized_id, 1122334455667788ULL);
}

// Corpse: Individual Fields Roundtrip

TEST(CorpseSerializeDeserializeRoundtrip, IndividualFields) {
	constexpr std::uint64_t test_id  = 123456789ULL;
	const auto              id_bytes = CorpseSRI::serialize_id(test_id);
	std::vector<std::byte>  id_buf(id_bytes.begin(), id_bytes.end());
	EXPECT_EQ(CorpseResponseDSI::get_id(id_buf), test_id);

	const RawMeat test_meat{42.5f};
	const auto    meat_bytes = CorpseSRI::serialize_raw_meat(test_meat);
	// Place with appropriate offset to test reading at TO_GET_RAW_MEAT_OFFSET
	std::vector<std::byte> meat_buf(CorpseSRI::CORPSE_BYTES, std::byte{0});
	Deserializer::append_bytes(meat_buf, meat_bytes, CorpseSRI::TO_GET_RAW_MEAT_OFFSET);
	EXPECT_FLOAT_EQ(CorpseResponseDSI::get_raw_meat(meat_buf).value, test_meat.value);

	const Vec3             test_pos{1.5, 2.5, 3.5};
	const auto             pos_bytes = CorpseSRI::serialize_position(test_pos);
	std::vector<std::byte> pos_buf(CorpseSRI::CORPSE_BYTES, std::byte{0});
	Deserializer::append_bytes(pos_buf, pos_bytes, CorpseSRI::TO_GET_POSITION_OFFSET);
	EXPECT_EQ(CorpseResponseDSI::get_position(pos_buf), test_pos);

	const Size test_size{.lateral = Lateral{2.0}, .height = Height{3.0}, .depth = Depth{4.0}};
	const auto size_bytes = CorpseSRI::serialize_size(test_size);
	std::vector<std::byte> size_buf(CorpseSRI::CORPSE_BYTES, std::byte{0});
	Deserializer::append_bytes(size_buf, size_bytes, CorpseSRI::TO_GET_SIZE_OFFSET);
	const auto deserialized_size = CorpseResponseDSI::get_size(size_buf);
	EXPECT_DOUBLE_EQ(deserialized_size.lateral.value, test_size.lateral.value);
	EXPECT_DOUBLE_EQ(deserialized_size.height.value, test_size.height.value);
	EXPECT_DOUBLE_EQ(deserialized_size.depth.value, test_size.depth.value);
}

// Corpse: Full Corpse Roundtrip

TEST(CorpseSerializeDeserializeRoundtrip, FullCorpseRoundtrip) {
	for (int iteration = 0; iteration < 10; ++iteration) {
		SCOPED_TRACE(::testing::Message() << "iteration " << iteration);

		const auto rand_id   = RandomGenerators::generate<std::uint64_t>();
		const auto rand_meat = RandomGenerators::generate<float>();
		const auto rand_pos  = RandomGenerators::generate<Vec3>();
		const Size rand_size{.lateral = Lateral{RandomGenerators::generate<double>()},
							 .height  = Height{RandomGenerators::generate<double>()},
							 .depth   = Depth{RandomGenerators::generate<double>()}};

		const Corpse original(rand_size, rand_pos, RawMeat{rand_meat}, rand_id);
		const auto   bytes = CorpseSRI::serialize_corpse(original);

		const std::vector<std::byte> buffer(bytes.begin(), bytes.end());
		const auto                   deserialized = CorpseResponseDSI::get_corpse(buffer);

		EXPECT_EQ(deserialized.id, original.id);
		EXPECT_FLOAT_EQ(deserialized.raw_meat.value, original.raw_meat.value);
		EXPECT_EQ(deserialized.position, original.position);
		EXPECT_DOUBLE_EQ(deserialized.size.lateral.value, original.size.lateral.value);
		EXPECT_DOUBLE_EQ(deserialized.size.height.value, original.size.height.value);
		EXPECT_DOUBLE_EQ(deserialized.size.depth.value, original.size.depth.value);
	}
}

// Corpse: Stream With Offsets

TEST(CorpseSerializeDeserializeRoundtrip, StreamWithOffsets) {
	const Size   size{.lateral = Lateral{1.0}, .height = Height{2.0}, .depth = Depth{3.0}};
	const Vec3   pos{10.0, 20.0, 30.0};
	const Corpse corpse(size, pos, RawMeat{50.0f}, 9999ULL);

	std::vector<std::byte> stream = {std::byte{0xDE},
									 std::byte{0xAD},
									 std::byte{0xBE},
									 std::byte{0xEF}};
	const size_t           offset = stream.size();

	const auto corpse_bytes = CorpseSRI::serialize_corpse(corpse);
	append_to_buffer(stream, corpse_bytes);
	stream.push_back(std::byte{0xFF});

	const auto deserialized = CorpseResponseDSI::get_corpse(stream, offset);

	EXPECT_EQ(deserialized.id, corpse.id);
	EXPECT_FLOAT_EQ(deserialized.raw_meat.value, corpse.raw_meat.value);
	EXPECT_EQ(deserialized.position, corpse.position);
	EXPECT_DOUBLE_EQ(deserialized.size.lateral.value, corpse.size.lateral.value);
	EXPECT_DOUBLE_EQ(deserialized.size.height.value, corpse.size.height.value);
	EXPECT_DOUBLE_EQ(deserialized.size.depth.value, corpse.size.depth.value);
}

// Corpse: Insufficient Buffer Size Throws Error

TEST(CorpseSerializeDeserializeRoundtrip, InsufficientBufferSizeThrowsError) {
	const std::vector<std::byte> empty_buffer;

	// Each buffer is one byte too short for the specific getter's required range
	const std::vector<std::byte> short_for_id(CorpseSRI::TO_GET_ID_OFFSET, std::byte{0x01});
	const std::vector<std::byte> short_for_raw_meat(CorpseSRI::TO_GET_RAW_MEAT_OFFSET
														+ CorpseSRI::RAW_MEAT_BYTES - 1,
													std::byte{0x01});
	const std::vector<std::byte> short_for_position(CorpseSRI::TO_GET_POSITION_OFFSET
														+ CorpseSRI::POSITION_BYTES - 1,
													std::byte{0x01});
	const std::vector<std::byte> short_for_size(CorpseSRI::TO_GET_SIZE_OFFSET
													+ CorpseSRI::SIZE_BYTES - 1,
												std::byte{0x01});

	EXPECT_THROW(CorpseResponseDSI::get_id(empty_buffer), ByteError);
	EXPECT_THROW(CorpseResponseDSI::get_raw_meat(short_for_raw_meat), ByteError);
	EXPECT_THROW(CorpseResponseDSI::get_position(short_for_position), ByteError);
	EXPECT_THROW(CorpseResponseDSI::get_size(short_for_size), ByteError);
	EXPECT_THROW(CorpseResponseDSI::get_corpse(short_for_size), ByteError);
}

// Corpse: Request DSI

TEST(CorpseSerializeDeserializeRoundtrip, RequestDSI) {
	constexpr std::uint64_t      corpse_id = 88887777ULL;
	const auto                   bytes     = Serializer::convert_uint64_t(corpse_id);
	const std::vector<std::byte> buffer(bytes.begin(), bytes.end());

	const auto interpreted_id = CorpseRequestDSI::interpret_like_get_corpse(buffer);
	EXPECT_EQ(interpreted_id, corpse_id);
}

// Territory: Full Territory Roundtrip

TEST(TerritorySerializeDeserializeRoundtrip, FullTerritoryRoundtrip) {
	for (int iteration = 0; iteration < 10; ++iteration) {
		SCOPED_TRACE(::testing::Message() << "iteration " << iteration);

		const Size rand_size{.lateral = Lateral{RandomGenerators::generate<double>()},
							 .height  = Height{RandomGenerators::generate<double>()},
							 .depth   = Depth{RandomGenerators::generate<double>()}};

		const Territory original(rand_size);
		const auto      bytes = TerritorySRI::serialize_territory(original);

		const std::vector<std::byte> buffer(bytes.begin(), bytes.end());
		const auto                   deserialized = TerritoryDSI::deserialize_territory(buffer);

		EXPECT_DOUBLE_EQ(deserialized.size().lateral.value, original.size().lateral.value);
		EXPECT_DOUBLE_EQ(deserialized.size().height.value, original.size().height.value);
		EXPECT_DOUBLE_EQ(deserialized.size().depth.value, original.size().depth.value);
	}
}

// Territory: Stream With Offsets

TEST(TerritorySerializeDeserializeRoundtrip, StreamWithOffsets) {
	const Size      size{.lateral = Lateral{10.0}, .height = Height{20.0}, .depth = Depth{30.0}};
	const Territory territory(size);

	std::vector<std::byte> stream = {std::byte{0xDE},
									 std::byte{0xAD},
									 std::byte{0xBE},
									 std::byte{0xEF}};
	const size_t           offset = stream.size();

	const auto territory_bytes = TerritorySRI::serialize_territory(territory);
	append_to_buffer(stream, territory_bytes);
	stream.push_back(std::byte{0xFF});

	const auto deserialized = TerritoryDSI::deserialize_territory(stream, offset);

	EXPECT_DOUBLE_EQ(deserialized.size().lateral.value, territory.size().lateral.value);
	EXPECT_DOUBLE_EQ(deserialized.size().height.value, territory.size().height.value);
	EXPECT_DOUBLE_EQ(deserialized.size().depth.value, territory.size().depth.value);
}

// Territory: Insufficient Buffer Size Throws Error

TEST(TerritorySerializeDeserializeRoundtrip, InsufficientBufferSizeThrowsError) {
	const std::vector<std::byte> empty_buffer;
	const std::vector<std::byte> short_buffer(TerritorySRI::TERRITORY_BYTES - 1, std::byte{0x01});

	EXPECT_THROW(TerritoryDSI::deserialize_territory(empty_buffer), ByteError);
	EXPECT_THROW(TerritoryDSI::deserialize_territory(short_buffer), ByteError);
}