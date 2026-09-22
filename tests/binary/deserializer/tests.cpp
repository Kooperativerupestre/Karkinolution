#include <gtest/gtest.h>
#include <karkinolution/binary/deserialization/deserializer.hpp>

TEST(DeserializerRead, Uint32_t) {
	std::vector<std::byte> bytes;
	EXPECT_THROW(Deserializer::read_uint32_t(bytes, 0), ByteError);
	EXPECT_THROW(Deserializer::read_uint32_t(bytes, bytes.size() + 10), ByteError);

	for (size_t i = 0; i < sizeof(uint32_t); i++) {
		bytes.push_back(static_cast<std::byte>(0x01));
	}
	Deserializer::read_uint32_t(bytes, 0);
	EXPECT_THROW(Deserializer::read_uint32_t(bytes, bytes.size() + 10), ByteError);
}

TEST(DeserializerRead, Uint64_t) {
	std::vector<std::byte> bytes;
	EXPECT_THROW(Deserializer::read_uint64_t(bytes, 0), ByteError);
	EXPECT_THROW(Deserializer::read_uint64_t(bytes, bytes.size() + 10), ByteError);

	for (size_t i = 0; i < sizeof(uint64_t); i++) {
		bytes.push_back(static_cast<std::byte>(0x01));
	}
	Deserializer::read_uint64_t(bytes, 0);
	EXPECT_THROW(Deserializer::read_uint64_t(bytes, bytes.size() + 10), ByteError);
}

TEST(DeserializerRead, Double) {
	std::vector<std::byte> bytes;
	EXPECT_THROW(Deserializer::read_double(bytes, 0), ByteError);
	EXPECT_THROW(Deserializer::read_double(bytes, bytes.size() + 10), ByteError);

	for (size_t i = 0; i < sizeof(double); i++) {
		bytes.push_back(static_cast<std::byte>(0x01));
	}
	Deserializer::read_double(bytes, 0);
	EXPECT_THROW(Deserializer::read_double(bytes, bytes.size() + 10), ByteError);
}

TEST(DeserializerRead, Uint8_t) {
	std::vector<std::byte> bytes;
	EXPECT_THROW(Deserializer::read_uint8_t(bytes, 0), ByteError);
	EXPECT_THROW(Deserializer::read_uint8_t(bytes, bytes.size() + 10), ByteError);

	for (size_t i = 0; i < sizeof(uint8_t); i++) {
		bytes.push_back(static_cast<std::byte>(0x01));
	}

	Deserializer::read_uint8_t(bytes, 0);
	EXPECT_THROW(Deserializer::read_uint8_t(bytes, bytes.size() + 10), ByteError);
}

TEST(DeserializerRead, String) {
	std::vector<std::byte> bytes;

	constexpr size_t length = 256;

	for (size_t i = 1; i < 256; i++) {
		EXPECT_THROW(Deserializer::read_string(bytes, 0, i), ByteError);
	}

	for (size_t i = 0; i < length; i++) {
		bytes.push_back(static_cast<std::byte>(0x01));
	}

	Deserializer::read_string(bytes, 0, length);
	Deserializer::read_string(bytes, length - 20, 0);
	EXPECT_THROW(Deserializer::read_string(bytes, 1, length), ByteError);
	EXPECT_THROW(Deserializer::read_string(bytes, 0, length * 2), ByteError);
}