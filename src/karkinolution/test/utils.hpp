#pragma once
#include "karkinolution/core/error.hpp"
#include "karkinolution/core/id.hpp"
#include "karkinolution/networking/sdk/src/client/client.hpp"

#include <karkinolution/binary/frames/motor.hpp>
#include <karkinolution/utils/k_random.hpp>
#include <karkinolution/world/world.hpp>
#include <random>
#include <string>

namespace TestUtils {
	inline std::string random_string() {
		static constexpr char CHARSET[] =
			"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

		std::uniform_int_distribution<size_t> length_dist(1, 64);
		std::uniform_int_distribution<size_t> char_dist(0, sizeof(CHARSET) - 2);

		std::string result;
		size_t      length = length_dist(gen);
		result.reserve(length);

		for (size_t i = 0; i < length; i++) {
			result.push_back(CHARSET[char_dist(gen)]);
		}

		return result;
	}

	template <typename Container>
	void append_to_buffer(std::vector<std::byte> &buffer, const Container &bytes) {
		for (const auto &byte : bytes) {
			buffer.push_back(static_cast<std::byte>(byte));
		}
	}

	inline void append_to_buffer(std::vector<std::byte> &buffer, std::byte byte) {
		buffer.push_back(byte);
	}

	inline void add_trash(std::vector<std::byte> &bytes, std::size_t i = 1) {
		if (i < 1) {
			throw SimulationError("Invalid number of trash additions");
		}

		for (size_t i = 0; i < 7; i++) {
			bytes.push_back(static_cast<std::byte>(RandomGenerators::generate<uint8_t>()));
		}
	}

	World make_small_world();

	std::vector<std::byte> make_see_creature_frame(Frames::Types::RequestId id,
												   BaseIdType               creature_id);
	KarkinolutionSDK::Client::ClientRequestData
	make_see_creature_request_data(BaseIdType creature_id);
} // namespace TestUtils