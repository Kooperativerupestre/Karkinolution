#pragma once

#include "karkinolution/networking/client_side/error.hpp"
#include "karkinolution/networking/frame/world/aliases.hpp"
#include "karkinolution/networking/frame/world/contract.hpp"
#include "karkinolution/networking/frame/world/models.hpp"

#include <cstdint>
#include <format>

namespace Karkinolution::Networking::ClientSide::Interpreters {
	using Karkinolution::Networking::Frames::World::Models::Request::CreateExtra;
	using Karkinolution::Networking::Frames::World::Models::Request::CycleExtra;
	using Karkinolution::Networking::Frames::World::Models::Request::EraseExtra;
	using Karkinolution::Networking::Frames::World::Models::Request::SeeExtra;
	using Karkinolution::Networking::Frames::World::Models::Request::WantExtra;
	using Karkinolution::Networking::Frames::World::Request::SeeTarget;

	inline const ResponseFrame &require_response_frame(const ResponseFrameVariant &frame) {
		if (const auto* response = std::get_if<ResponseFrame>(&frame)) {
			return *response;
		}

		const auto &error = std::get<ErrorFrame>(frame);
		throw ClientError(
			std::format("Server returned error code {}", static_cast<std::uint8_t>(error.get_sub_type())));
	}
} // namespace Karkinolution::Networking::ClientSide::Interpreters
