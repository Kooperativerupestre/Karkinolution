#include <gtest/gtest.h>
#include <karkinolution/binary/deserializer.hpp>

TEST(DeserializerRead, Uint32_t) {
	std::vector<std::byte> bytes;
	EXPECT_THROW(Karkinolution::Binary::Deserializer::read_uint32_t(bytes, 0), Karkinolution::Binary::ByteError);
	EXPECT_THROW(Karkinolution::Binary::Deserializer::read_uint32_t(bytes, bytes.size() + 10), Karkinolution::Binary::ByteError);

	for (size_t i = 0; i < sizeof(uint32_t); i++) {
		bytes.push_back(static_cast<std::byte>(0x01));
	}
	Karkinolution::Binary::Deserializer::read_uint32_t(bytes, 0);
	EXPECT_THROW(Karkinolution::Binary::Deserializer::read_uint32_t(bytes, bytes.size() + 10), Karkinolution::Binary::ByteError);
}

TEST(DeserializerRead, Uint64_t) {
	std::vector<std::byte> bytes;
	EXPECT_THROW(Karkinolution::Binary::Deserializer::read_uint64_t(bytes, 0), Karkinolution::Binary::ByteError);
	EXPECT_THROW(Karkinolution::Binary::Deserializer::read_uint64_t(bytes, bytes.size() + 10), Karkinolution::Binary::ByteError);

	for (size_t i = 0; i < sizeof(uint64_t); i++) {
		bytes.push_back(static_cast<std::byte>(0x01));
	}
	Karkinolution::Binary::Deserializer::read_uint64_t(bytes, 0);
	EXPECT_THROW(Karkinolution::Binary::Deserializer::read_uint64_t(bytes, bytes.size() + 10), Karkinolution::Binary::ByteError);
}

TEST(DeserializerRead, Double) {
	std::vector<std::byte> bytes;
	EXPECT_THROW(Karkinolution::Binary::Deserializer::read_double(bytes, 0), Karkinolution::Binary::ByteError);
	EXPECT_THROW(Karkinolution::Binary::Deserializer::read_double(bytes, bytes.size() + 10), Karkinolution::Binary::ByteError);

	for (size_t i = 0; i < sizeof(double); i++) {
		bytes.push_back(static_cast<std::byte>(0x01));
	}
	Karkinolution::Binary::Deserializer::read_double(bytes, 0);
	EXPECT_THROW(Karkinolution::Binary::Deserializer::read_double(bytes, bytes.size() + 10), Karkinolution::Binary::ByteError);
}

TEST(DeserializerRead, Uint8_t) {
	std::vector<std::byte> bytes;
	EXPECT_THROW(Karkinolution::Binary::Deserializer::read_uint8_t(bytes, 0), Karkinolution::Binary::ByteError);
	EXPECT_THROW(Karkinolution::Binary::Deserializer::read_uint8_t(bytes, bytes.size() + 10), Karkinolution::Binary::ByteError);

	for (size_t i = 0; i < sizeof(uint8_t); i++) {
		bytes.push_back(static_cast<std::byte>(0x01));
	}

	Karkinolution::Binary::Deserializer::read_uint8_t(bytes, 0);
	EXPECT_THROW(Karkinolution::Binary::Deserializer::read_uint8_t(bytes, bytes.size() + 10), Karkinolution::Binary::ByteError);
}

TEST(DeserializerRead, String) {
	std::vector<std::byte> bytes;

	constexpr size_t length = 256;

	for (size_t i = 1; i < 256; i++) {
		EXPECT_THROW(Karkinolution::Binary::Deserializer::read_string(bytes, 0, i), Karkinolution::Binary::ByteError);
	}

	for (size_t i = 0; i < length; i++) {
		bytes.push_back(static_cast<std::byte>(0x01));
	}

	Karkinolution::Binary::Deserializer::read_string(bytes, 0, length);
	Karkinolution::Binary::Deserializer::read_string(bytes, length - 20, 0);
	EXPECT_THROW(Karkinolution::Binary::Deserializer::read_string(bytes, 1, length), Karkinolution::Binary::ByteError);
	EXPECT_THROW(Karkinolution::Binary::Deserializer::read_string(bytes, 0, length * 2), Karkinolution::Binary::ByteError);
}