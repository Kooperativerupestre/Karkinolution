#include "karkinolution/networking/frame/world/core_aliases.hpp"
#include "karkinolution/networking/frame/world/models.hpp"
#include "karkinolution/networking/raw_frame_buffer.hpp"
#include "karkinolution/networking/scope/layout.hpp"

#include <gtest/gtest.h>
#include <karkinolution/networking/frame/world/motor.hpp>
#include <vector>

using namespace Karkinolution;

std::vector<std::byte> make_random_raw_request() {
	return Karkinolution::Networking::Frames::World::Motor::make_request_raw_bytes(
		Karkinolution::Networking::Frames::World::RequestSubTypes::CREATE,
		3,
		Karkinolution::Networking::Frames::World::Resource::CORPSE,
		std::vector<std::byte>{static_cast<std::byte>(0x01)},
		Karkinolution::Networking::Frames::World::Models::Request::CreateExtra{});
}

TEST(RawFrameBufferAnalyzer, GetEndOfFrame) {

	{
		std::vector<std::byte> incomplete_declared_size{};

		incomplete_declared_size.push_back(static_cast<std::byte>(0x1));

		EXPECT_ANY_THROW(
			Karkinolution::Networking::FrameBufferAnalyzer::get_size_of_frame(incomplete_declared_size,
																			  Networking::Scope::Scope::SERVER));
		EXPECT_ANY_THROW(Networking::FrameBufferAnalyzer::get_size_of_frame(incomplete_declared_size,
																			Networking::Scope::Scope::WORLD));

		// Tests whether a frame with an incomplete declared size throws an error when we try to retrieve its bytes.
	}

	{
		std::vector<std::byte> raw_request = make_random_raw_request();
		EXPECT_EQ(raw_request.size(),
				  Networking::FrameBufferAnalyzer::get_end_of_frame(raw_request, Networking::Scope::Scope::WORLD));
	}

	{
		// Tests whether the end of an already complete frame in the buffer remains restricted to its actual end,
		// even when new bytes are added.

		Networking::RawFrameBuffer buffer;
		std::vector<std::byte>     new_bytes;

		new_bytes.push_back(static_cast<std::byte>(0x02));
		new_bytes.push_back(static_cast<std::byte>(0x03));
		new_bytes.push_back(static_cast<std::byte>(0x04));

		const auto raw_frame = make_random_raw_request();


		for (size_t i = 0; i < raw_frame.size(); i++) {
			buffer.add(raw_frame[i]);
		}

		const auto expected_end_of_buffer = buffer.view().size();


		buffer.add(static_cast<std::byte>(0x03));
		// Add garbage to the buffer, ensuring that the result is based on the actual frame position rather than simply
		// the end of the buffer.

		const auto end_of_frame =
			Networking::FrameBufferAnalyzer::get_end_of_frame(buffer, new_bytes, Networking::Scope::Scope::WORLD);

		ASSERT_TRUE(end_of_frame);
		ASSERT_EQ(end_of_frame.value().place, Networking::FrameBufferAnalyzer::PlacesOfEndOfFrame::BUFFER);
		ASSERT_EQ(end_of_frame->index, expected_end_of_buffer);
	}
}