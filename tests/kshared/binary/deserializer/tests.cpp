#include <gtest/gtest.h>
#include <shared/binary/deserializer.hpp>

TEST(DeserializerRead, Uint32_t) {
	std::vector<std::byte> bytes;
	EXPECT_THROW(KShared::Binary::Deserializer::read_uint32_t(bytes, 0), KShared::Binary::ByteError);
	EXPECT_THROW(KShared::Binary::Deserializer::read_uint32_t(bytes, bytes.size() + 10), KShared::Binary::ByteError);

	for (size_t i = 0; i < sizeof(uint32_t); i++) {
		bytes.push_back(static_cast<std::byte>(0x01));
	}
	KShared::Binary::Deserializer::read_uint32_t(bytes, 0);
	EXPECT_THROW(KShared::Binary::Deserializer::read_uint32_t(bytes, bytes.size() + 10), KShared::Binary::ByteError);
}

TEST(DeserializerRead, Uint64_t) {
	std::vector<std::byte> bytes;
	EXPECT_THROW(KShared::Binary::Deserializer::read_uint64_t(bytes, 0), KShared::Binary::ByteError);
	EXPECT_THROW(KShared::Binary::Deserializer::read_uint64_t(bytes, bytes.size() + 10), KShared::Binary::ByteError);

	for (size_t i = 0; i < sizeof(uint64_t); i++) {
		bytes.push_back(static_cast<std::byte>(0x01));
	}
	KShared::Binary::Deserializer::read_uint64_t(bytes, 0);
	EXPECT_THROW(KShared::Binary::Deserializer::read_uint64_t(bytes, bytes.size() + 10), KShared::Binary::ByteError);
}

TEST(DeserializerRead, Double) {
	std::vector<std::byte> bytes;
	EXPECT_THROW(KShared::Binary::Deserializer::read_double(bytes, 0), KShared::Binary::ByteError);
	EXPECT_THROW(KShared::Binary::Deserializer::read_double(bytes, bytes.size() + 10), KShared::Binary::ByteError);

	for (size_t i = 0; i < sizeof(double); i++) {
		bytes.push_back(static_cast<std::byte>(0x01));
	}
	KShared::Binary::Deserializer::read_double(bytes, 0);
	EXPECT_THROW(KShared::Binary::Deserializer::read_double(bytes, bytes.size() + 10), KShared::Binary::ByteError);
}

TEST(DeserializerRead, Uint8_t) {
	std::vector<std::byte> bytes;
	EXPECT_THROW(KShared::Binary::Deserializer::read_uint8_t(bytes, 0), KShared::Binary::ByteError);
	EXPECT_THROW(KShared::Binary::Deserializer::read_uint8_t(bytes, bytes.size() + 10), KShared::Binary::ByteError);

	for (size_t i = 0; i < sizeof(uint8_t); i++) {
		bytes.push_back(static_cast<std::byte>(0x01));
	}

	KShared::Binary::Deserializer::read_uint8_t(bytes, 0);
	EXPECT_THROW(KShared::Binary::Deserializer::read_uint8_t(bytes, bytes.size() + 10), KShared::Binary::ByteError);
}

TEST(DeserializerRead, String) {
	std::vector<std::byte> bytes;

	constexpr size_t length = 256;

	for (size_t i = 1; i < 256; i++) {
		EXPECT_THROW(KShared::Binary::Deserializer::read_string(bytes, 0, i), KShared::Binary::ByteError);
	}

	for (size_t i = 0; i < length; i++) {
		bytes.push_back(static_cast<std::byte>(0x01));
	}

	KShared::Binary::Deserializer::read_string(bytes, 0, length);
	KShared::Binary::Deserializer::read_string(bytes, length - 20, 0);
	EXPECT_THROW(KShared::Binary::Deserializer::read_string(bytes, 1, length), KShared::Binary::ByteError);
	EXPECT_THROW(KShared::Binary::Deserializer::read_string(bytes, 0, length * 2), KShared::Binary::ByteError);
}

TEST(DeserializerRead, Int8_t) {
	std::vector<std::byte> bytes;
	EXPECT_THROW(KShared::Binary::Deserializer::read_int8_t(bytes, 0), KShared::Binary::ByteError);
	EXPECT_THROW(KShared::Binary::Deserializer::read_int8_t(bytes, 10), KShared::Binary::ByteError);

	static_assert(std::same_as<decltype(KShared::Binary::Deserializer::read_int8_t(bytes, 0)), std::int8_t>);
	static_assert(std::same_as<decltype(KShared::Binary::Deserializer::Types::Bytes1Deserialized(bytes, 0).as_int8_t()),
							   std::int8_t>);

	const std::vector<std::int8_t> test_values = {0, 1, -1, 127, -128, 42, -42};
	for (std::int8_t val : test_values) {
		std::vector<std::byte> buf = {static_cast<std::byte>(val)};
		EXPECT_EQ(KShared::Binary::Deserializer::read_int8_t(buf, 0), val);
		EXPECT_EQ(KShared::Binary::Deserializer::deserialize<std::int8_t>(buf, 0), val);
	}
}
