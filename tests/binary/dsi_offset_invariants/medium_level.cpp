#include <gtest/gtest.h>
#include <karkinolution/binary/byte_utils.hpp>
#include <karkinolution/binary/interpreters/unit.hpp>
#include <karkinolution/binary/interpreters/vec.hpp>
#include <shared/math/physic/vec/model.hpp>
#include <shared/math/units.hpp>

TEST(DSIOffsetInvariantsMidLevel, VecMidLevelAuxiliaryGettersAutoAddOffset) {
	const KShared::Math::Vec::Vec2 original{12.5, -45.0};
	const auto                     vec_bytes = VecSRI::serialize_vec(original);

	std::vector<std::byte> payload     = {std::byte{0xAA}, std::byte{0xBB}, std::byte{0xCC}};
	const std::size_t      base_offset = payload.size();
	Karkinolution::Binary::Utils::append_bytes(payload, vec_bytes);

	EXPECT_DOUBLE_EQ(VecDSI::get_x(payload, base_offset), original.x);
	EXPECT_DOUBLE_EQ(VecDSI::get_y(payload, base_offset), original.y);
}

TEST(DSIOffsetInvariantsMidLevel, UnitsSizeMidLevelAuxiliaryGettersAutoAddOffset) {
	const KShared::Math::Units::Size original{10.0, 20.0};
	const auto                       size_bytes = UnitsSRI::serialize_size(original);

	std::vector<std::byte> payload     = {std::byte{0xDE}, std::byte{0xAD}};
	const std::size_t      base_offset = payload.size();
	Karkinolution::Binary::Utils::append_bytes(payload, size_bytes);

	EXPECT_DOUBLE_EQ(UnitsDSI::deserialize_width(payload, base_offset).value, original.width.value);
	EXPECT_DOUBLE_EQ(UnitsDSI::deserialize_height(payload, base_offset).value, original.height.value);
}
