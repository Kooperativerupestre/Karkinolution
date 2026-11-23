#include <array>
#include <gtest/gtest.h>
#include <shared/binary/byte_utils.hpp>
#include <vector>

using namespace KShared::Binary;

TEST(ByteUtilsAppendBytes, ArrayValidAndOutOfBounds) {
	std::array<std::byte, 4> arr{};
	std::vector<std::byte>   payload = {std::byte{0x01}, std::byte{0x02}};

	Utils::append_bytes(arr, payload, 0);
	EXPECT_EQ(arr[0], std::byte{0x01});
	EXPECT_EQ(arr[1], std::byte{0x02});

	Utils::append_bytes(arr, std::byte{0x09}, 3);
	EXPECT_EQ(arr[3], std::byte{0x09});

	// Out of bounds
	EXPECT_THROW(Utils::append_bytes(arr, payload, 3), ByteError);
	EXPECT_THROW(Utils::append_bytes(arr, payload, 5), ByteError);
	EXPECT_THROW(Utils::append_bytes(arr, std::byte{0x09}, 4), ByteError);
}

TEST(ByteUtilsAppendBytes, VectorValidAndOutOfBounds) {
	std::vector<std::byte> vec(4, std::byte{0x00});
	std::vector<std::byte> payload = {std::byte{0xAA}, std::byte{0xBB}};

	Utils::append_bytes(vec, payload, 2);
	EXPECT_EQ(vec[2], std::byte{0xAA});
	EXPECT_EQ(vec[3], std::byte{0xBB});

	Utils::append_bytes(vec, std::byte{0xCC}, 0);
	EXPECT_EQ(vec[0], std::byte{0xCC});

	// Out of bounds
	EXPECT_THROW(Utils::append_bytes(vec, payload, 3), ByteError);
	EXPECT_THROW(Utils::append_bytes(vec, payload, 10), ByteError);
	EXPECT_THROW(Utils::append_bytes(vec, std::byte{0xFF}, 4), ByteError);
}
