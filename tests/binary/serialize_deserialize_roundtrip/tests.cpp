#include <gtest/gtest.h>
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
} // namespace

TEST(SerializeDeserializeRoundtrip, AllTypes) {
	for (int iteration = 0; iteration < 10; iteration++) {
		SCOPED_TRACE(::testing::Message() << "iteration " << iteration);

		const auto        original_u32 = RandomGenerators::generate<std::uint32_t>();
		const auto        original_u64 = RandomGenerators::generate<std::uint64_t>();
		const auto        original_dbl = RandomGenerators::generate<double>();
		const auto        original_u8  = RandomGenerators::generate<std::uint8_t>();
		const std::string original_str = random_string();

		std::vector<std::byte> buffer;

		const size_t offset_u32 = buffer.size();
		Deserializer::append_bytes(buffer, Serializer::convert_uint32_t(original_u32));

		const size_t offset_u64 = buffer.size();
		Deserializer::append_bytes(buffer, Serializer::convert_uint64_t(original_u64));

		const size_t offset_dbl = buffer.size();
		Deserializer::append_bytes(buffer, Serializer::convert_double(original_dbl));

		const size_t offset_u8 = buffer.size();
		Deserializer::append_bytes(buffer, Serializer::convert_uint8_t(original_u8));

		const size_t offset_str = buffer.size();
		Deserializer::append_bytes(buffer, Serializer::convert_string(original_str));

		EXPECT_EQ(Deserializer::read_uint32_t(buffer, offset_u32), original_u32);
		EXPECT_EQ(Deserializer::read_uint64_t(buffer, offset_u64), original_u64);
		EXPECT_EQ(Deserializer::read_double(buffer, offset_dbl), original_dbl);
		EXPECT_EQ(Deserializer::read_uint8_t(buffer, offset_u8), original_u8);
		EXPECT_EQ(Deserializer::read_string(buffer, offset_str, original_str.size()), original_str);
	}
}