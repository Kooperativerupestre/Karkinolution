#include <gtest/gtest.h>
#include <karkinolution/binary/byte_utils.hpp>
#include <karkinolution/binary/interpreters/unit.hpp>
#include <karkinolution/binary/interpreters/vec.hpp>
#include <karkinolution/math/physic/vec/model.hpp>
#include <karkinolution/math/units.hpp>

TEST(DSIOffsetInvariantsMidLevel, VecMidLevelAuxiliaryGettersAutoAddOffset) {
	const Vec3 original{12.5, -45.0, 100.25};
	const auto vec_bytes = VecSRI::serialize_vec(original);

	std::vector<std::byte> payload     = {std::byte{0xAA}, std::byte{0xBB}, std::byte{0xCC}};
	const std::size_t      base_offset = payload.size();
	BinaryUtils::append_bytes(payload, vec_bytes);

	EXPECT_DOUBLE_EQ(VecDSI::get_x(payload, base_offset), original.x);
	EXPECT_DOUBLE_EQ(VecDSI::get_y(payload, base_offset), original.y);
	EXPECT_DOUBLE_EQ(VecDSI::get_z(payload, base_offset), original.z);
}

TEST(DSIOffsetInvariantsMidLevel, UnitsSizeMidLevelAuxiliaryGettersAutoAddOffset) {
	const Size original{10.0, 20.0, 30.0};
	const auto size_bytes = UnitsSRI::serialize_size(original);

	std::vector<std::byte> payload     = {std::byte{0xDE}, std::byte{0xAD}};
	const std::size_t      base_offset = payload.size();
	BinaryUtils::append_bytes(payload, size_bytes);

	EXPECT_DOUBLE_EQ(UnitsDSI::deserialize_lateral(payload, base_offset).value,
					 original.lateral.value);
	EXPECT_DOUBLE_EQ(UnitsDSI::deserialize_height(payload, base_offset).value,
					 original.height.value);
	EXPECT_DOUBLE_EQ(UnitsDSI::deserialize_depth(payload, base_offset).value, original.depth.value);
}
