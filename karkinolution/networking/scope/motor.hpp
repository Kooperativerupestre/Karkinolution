#pragma once

#include "karkinolution/binary/byte_range.hpp"
#include "karkinolution/networking/scope/layout.hpp"

#include <vector>

namespace Karkinolution::Networking::Scope::Motor {
	template <Binary::ByteRange Payload> std::vector<std::byte> make_frame(Scope scope, const Payload &payload) {
		std::vector<std::byte> frame;
		frame.reserve(payload.size() + 1);
		frame.push_back(static_cast<std::byte>(scope));
		frame.insert(frame.end(), payload.begin(), payload.end());
		return frame;
	}

	template <Binary::ByteRange Payload> std::vector<std::byte> make_system_frame(const Payload &payload) {
		return make_frame(Scope::SERVER, payload);
	}

	template <Binary::ByteRange Payload> std::vector<std::byte> make_world_frame(const Payload &payload) {
		return make_frame(Scope::WORLD, payload);
	}
} // namespace Karkinolution::Networking::Scope::Motor
