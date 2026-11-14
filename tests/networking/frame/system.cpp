#include "karkinolution/networking/frame/interpreter.hpp"
#include "karkinolution/networking/frame/motor.hpp"
#include "karkinolution/networking/frame/parser/parser.hpp"

#include <gtest/gtest.h>

namespace {
	using namespace Karkinolution::Networking::Frames;

	constexpr SystemCall::Id call_id = 42;
} // namespace

TEST(SystemFrames, RawSystemCallRoundTripsItsTypeSubtypeIdAndPayload) {
	const auto payload = Interpreter::SystemCall::SRI::make_config(123);
	const auto raw     = Motor::make_raw_system_call(SystemCallSubTypes::CONFIG, call_id, payload);

	ASSERT_EQ(Parser::has_frame(raw, Layout::SystemCall::FIXED_HEADER_SIZE), Parser::FrameExistence::YES);
	const auto parsed = Parser::parse_system_call_frame(raw);

	EXPECT_EQ(parsed.get_id(), call_id);
	EXPECT_EQ(parsed.get_sub_type(), SystemCallSubTypes::CONFIG);
	EXPECT_EQ(Interpreter::SystemCall::DSI::get_config(parsed.get_payload()), 123);
}

TEST(SystemFrames, RawSystemOutputRoundTripsItsTypeSubtypeCallIdAndPayload) {
	const auto payload = Interpreter::SystemOutput::SRI::make_config_output({.could_connect = true});
	const auto raw     = Motor::make_raw_system_output(SystemOutputSubTypes::ConfigOutput, call_id, payload);

	ASSERT_EQ(Parser::has_frame(raw, Layout::SystemOutput::FIXED_HEADER_SIZE), Parser::FrameExistence::YES);
	const auto parsed = Parser::parse_system_output_frame(raw);

	EXPECT_EQ(parsed.get_call_id(), call_id);
	EXPECT_EQ(parsed.get_sub_type(), SystemOutputSubTypes::ConfigOutput);
	EXPECT_TRUE(Interpreter::DSI::get_config_output(parsed.get_payload()).could_connect);
}
