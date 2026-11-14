#include "karkinolution/networking/frame/motor.hpp"
#include "karkinolution/networking/scope/layout.hpp"
#include "karkinolution/networking/scope/motor.hpp"
#include "karkinolution/networking/scope/parser.hpp"

#include <gtest/gtest.h>

namespace {
	using namespace Karkinolution::Networking;
} // namespace

TEST(ScopeFrames, PrefixesServerAndWorldFramesWithOnlyTheirScopeByte) {
	const std::vector<std::byte> payload{std::byte{0x11}, std::byte{0x22}};

	const auto server_frame = Scope::Motor::make_system_frame(payload);
	const auto world_frame  = Scope::Motor::make_world_frame(payload);

	ASSERT_EQ(server_frame.size(), payload.size() + 1);
	ASSERT_EQ(world_frame.size(), payload.size() + 1);
	EXPECT_EQ(Scope::Parser::get_type(server_frame), Scope::Scope::SERVER);
	EXPECT_EQ(Scope::Parser::get_type(world_frame), Scope::Scope::WORLD);
	EXPECT_EQ(std::vector<std::byte>(server_frame.begin() + 1, server_frame.end()), payload);
	EXPECT_EQ(std::vector<std::byte>(world_frame.begin() + 1, world_frame.end()), payload);
}

TEST(ScopeParser, RejectsEmptyAndUnknownScopeFrames) {
	EXPECT_THROW(Scope::Parser::get_type(std::vector<std::byte>{}), Karkinolution::Binary::ByteError);
	EXPECT_THROW(Scope::Parser::get_type(std::vector<std::byte>{std::byte{0xFF}}), Karkinolution::Binary::ByteError);
}
