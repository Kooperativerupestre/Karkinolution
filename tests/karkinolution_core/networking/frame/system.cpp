#include "karkinolution/networking/frame/system/interpreter.hpp"
#include "karkinolution/networking/frame/system/motor.hpp"
#include "karkinolution/networking/frame/system/parser/internal.hpp"
#include "karkinolution/networking/frame/system/parser/parser.hpp"

#include <gtest/gtest.h>

namespace {
	using namespace Karkinolution::Networking::Frames::System;

	constexpr SystemCall::Id call_id = 42;
} // namespace

TEST(SystemFrames, RawSystemCallRoundTripsItsTypeSubtypeIdAndPayload) {
	const auto payload = Interpreter::SystemCall::SRI::make_config(123);
	const auto raw     = Motor::make_raw_system_call(SystemCallSubTypes::CONFIG, call_id, payload);

	ASSERT_EQ(Parser::has_frame(raw), Parser::Internal::FrameExistence::YES);
	const auto parsed = Parser::parse_system_call_frame(raw);

	EXPECT_EQ(parsed.get_id(), call_id);
	EXPECT_EQ(parsed.get_sub_type(), SystemCallSubTypes::CONFIG);
	EXPECT_EQ(Interpreter::SystemCall::DSI::get_config(parsed.get_payload()), 123);
}

TEST(SystemFrames, RawSystemOutputRoundTripsItsTypeSubtypeCallIdAndPayload) {
	const auto payload = Interpreter::SystemOutput::SRI::make_config_output({.could_connect = true});
	const auto raw     = Motor::make_raw_system_output(SystemOutputSubTypes::ConfigOutput, call_id, payload);

	ASSERT_EQ(Parser::has_frame(raw), Parser::Internal::FrameExistence::YES);
	const auto parsed = Parser::parse_system_output_frame(raw);

	EXPECT_EQ(parsed.get_call_id(), call_id);
	EXPECT_EQ(parsed.get_sub_type(), SystemOutputSubTypes::ConfigOutput);
	EXPECT_TRUE(Interpreter::DSI::get_config_output(parsed.get_payload()).could_connect);
}

TEST(SystemFrames, CreateWorldPreservesItsAutoConnectHeaderAndPresetPayload) {
	const Karkinolution::World::WorldPreset preset{
		.seed    = 42,
		.size    = {.width = KShared::Math::Units::Width{100.0}, .height = KShared::Math::Units::Height{200.0}},
		.epsilon = 5.0f};
	const auto payload = Interpreter::SystemCall::SRI::make_create_world(preset);
	const auto raw     = Motor::make_raw_system_call(SystemCallSubTypes::CREATE_WORLD,
                                                 call_id,
                                                 payload,
                                                 Models::SystemCall::CreateWorldExtra{.auto_connect = true});

	ASSERT_EQ(Parser::has_frame(raw), Parser::Internal::FrameExistence::YES);
	const auto parsed = Parser::parse_system_call_frame(raw);
	ASSERT_TRUE(std::holds_alternative<Models::SystemCall::CreateWorldExtra>(parsed.get_extra()));
	EXPECT_TRUE(std::get<Models::SystemCall::CreateWorldExtra>(parsed.get_extra()).auto_connect);
	EXPECT_EQ(Interpreter::SystemCall::DSI::get_create_world(parsed.get_payload()).seed, preset.seed);
	EXPECT_DOUBLE_EQ(Interpreter::SystemCall::DSI::get_create_world(parsed.get_payload()).size.width.value,
					 preset.size.width.value);
}

TEST(SystemFrames, WorldCreatedOutputRoundTripsItsBooleanPayload) {
	const auto payload = Interpreter::SystemOutput::SRI::make_world_created({.world_created = true});
	const auto raw     = Motor::make_raw_system_output(SystemOutputSubTypes::WorldCreated, call_id, payload);

	ASSERT_EQ(Parser::has_frame(raw), Parser::Internal::FrameExistence::YES);
	const auto parsed = Parser::parse_system_output_frame(raw);
	EXPECT_EQ(parsed.get_sub_type(), SystemOutputSubTypes::WorldCreated);
	EXPECT_TRUE(Interpreter::DSI::get_world_created(parsed.get_payload()).world_created);
}
