#include <gtest/gtest.h>
#include <karkinolution/binary/byte_utils.hpp>
#include <karkinolution/test/utils.hpp>

// ByteUtils: Zero Padding

TEST(ByteUtilsRoundtrip, ZeroPadding) {
	std::array<std::byte, 5> array = {std::byte{0x01},
									  std::byte{0x02},
									  std::byte{0x03},
									  std::byte{0x04},
									  std::byte{0x05}};

	Karkinolution::Binary::Utils::zero_padding(array, 2);
	EXPECT_EQ(array[0], std::byte{0x01});
	EXPECT_EQ(array[1], std::byte{0x02});
	EXPECT_EQ(array[2], std::byte{0x00});
	EXPECT_EQ(array[3], std::byte{0x00});
	EXPECT_EQ(array[4], std::byte{0x00});
	Karkinolution::Binary::Utils::zero_padding(array, 0);
	for (const auto &byte : array) {
		EXPECT_EQ(byte, std::byte{0x00});
	}

	std::vector<std::byte> vec = {std::byte{0xAA}, std::byte{0xBB}};
	Karkinolution::Binary::Utils::zero_padding(vec, 2);
	EXPECT_EQ(vec[0], std::byte{0xAA});
	EXPECT_EQ(vec[1], std::byte{0xBB});

	EXPECT_THROW(Karkinolution::Binary::Utils::zero_padding(vec, 3), ByteError);
}
