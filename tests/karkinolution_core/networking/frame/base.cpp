#include "karkinolution/networking/frame/base/parser.hpp"
#include "shared/binary/serializer.hpp"

#include <cstdint>
#include <gtest/gtest.h>
#include <vector>

namespace {

	std::vector<std::byte> make_header(std::uint32_t declared_size, std::uint8_t type, std::uint8_t sub_type) {
		const auto size_bytes = KShared::Binary::Serializer::Core::convert_4_bytes(declared_size);
		return {size_bytes[0],
				size_bytes[1],
				size_bytes[2],
				size_bytes[3],
				static_cast<std::byte>(type),
				static_cast<std::byte>(sub_type)};
	}

} // namespace

TEST(FrameBase, ParsesWorldTypeAndNestedSubtypeVariant) {
	const auto bytes = make_header(2, 1, 1);

	const auto type =
		Karkinolution::Networking::Frames::Base::Parser::get_type<Karkinolution::Networking::Frames::World::Type>(
			bytes);
	const auto sub_type =
		Karkinolution::Networking::Frames::Base::Parser::get_sub_type<Karkinolution::Networking::Frames::World::Type>(
			bytes);

	ASSERT_TRUE(std::holds_alternative<Karkinolution::Networking::Frames::World::Type>(type));
	EXPECT_EQ(std::get<Karkinolution::Networking::Frames::World::Type>(type),
			  Karkinolution::Networking::Frames::World::Type::Request);
	ASSERT_TRUE(std::holds_alternative<Karkinolution::Networking::Frames::World::Detail::SubTypeVariant>(sub_type));
	const auto &world_sub_type = std::get<Karkinolution::Networking::Frames::World::Detail::SubTypeVariant>(sub_type);
	EXPECT_EQ(std::get<Karkinolution::Networking::Frames::World::Request::SubType>(world_sub_type),
			  Karkinolution::Networking::Frames::World::Request::SubType::SEE);
}

TEST(FrameBase, ParsesSystemTypeAndNestedSubtypeVariant) {
	const auto bytes = make_header(2, 1, 4);

	const auto type =
		Karkinolution::Networking::Frames::Base::Parser::get_type<Karkinolution::Networking::Frames::System::Type>(
			bytes);
	const auto sub_type =
		Karkinolution::Networking::Frames::Base::Parser::get_sub_type<Karkinolution::Networking::Frames::System::Type>(
			bytes);

	ASSERT_TRUE(std::holds_alternative<Karkinolution::Networking::Frames::System::Type>(type));
	EXPECT_EQ(std::get<Karkinolution::Networking::Frames::System::Type>(type),
			  Karkinolution::Networking::Frames::System::Type::SystemOutput);
	ASSERT_TRUE(std::holds_alternative<Karkinolution::Networking::Frames::System::Detail::SubTypeVariant>(sub_type));
	const auto &system_sub_type = std::get<Karkinolution::Networking::Frames::System::Detail::SubTypeVariant>(sub_type);
	EXPECT_EQ(std::get<Karkinolution::Networking::Frames::System::SystemOutput::SubType>(system_sub_type),
			  Karkinolution::Networking::Frames::System::SystemOutput::SubType::ConfigOutput);
}

TEST(FrameBase, DetectsCommonFrameCompletenessFromDeclaredSize) {
	using Karkinolution::Networking::Frames::Base::FrameExistence;
	using Karkinolution::Networking::Frames::Base::Parser::has_frame;

	EXPECT_EQ(has_frame(std::vector<std::byte>{}), FrameExistence::NO);
	EXPECT_EQ(has_frame(std::vector<std::byte>{std::byte{0x00}}), FrameExistence::DECLARED_SIZE_INCOMPLETE);
	EXPECT_EQ(has_frame(make_header(2, 1, 1)), FrameExistence::YES);

	auto incomplete = make_header(5, 1, 1);
	EXPECT_EQ(has_frame(incomplete), FrameExistence::INCOMPLETE);
	incomplete.resize(9, std::byte{0});
	EXPECT_EQ(has_frame(incomplete), FrameExistence::YES);
}

TEST(FrameBase, RejectsDeclaredSizeShorterThanCommonHeader) {
	EXPECT_THROW(Karkinolution::Networking::Frames::Base::Parser::has_frame(make_header(1, 1, 1)),
				 KShared::Binary::ByteError);
}
