#include <cstdint>
#include <gtest/gtest.h>
#include <karkinolution/binary/types/list.hpp>
#include <vector>

TEST(ListSerialize, WritesHeaderAndElementsInOrder) {
	const std::vector<std::uint32_t> values = {0x01020304, 0xA1B2C3D4, 0x00FF00EE};

	const auto serialized =
		ListTSRI::serialize<sizeof(std::uint32_t)>(values, [](std::uint32_t value) {
			return Serializer::convert_uint32_t(value);
		});

	const std::vector<std::byte> expected = {
		std::byte{0x00},
		std::byte{0x03},
		std::byte{0x01},
		std::byte{0x02},
		std::byte{0x03},
		std::byte{0x04},
		std::byte{0xA1},
		std::byte{0xB2},
		std::byte{0xC3},
		std::byte{0xD4},
		std::byte{0x00},
		std::byte{0xFF},
		std::byte{0x00},
		std::byte{0xEE},
	};

	EXPECT_EQ(serialized, expected);
}
