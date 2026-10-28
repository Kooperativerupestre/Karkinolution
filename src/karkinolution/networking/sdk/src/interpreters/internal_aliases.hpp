#pragma once

#include "karkinolution/binary/frames/aliases.hpp"
#include "karkinolution/binary/frames/models.hpp"
#include "karkinolution/binary/protocol/contract.hpp"
#include "karkinolution/networking/sdk/src/error.hpp"

#include <cstdint>
#include <format>

namespace KarkinolutionSDK {
	namespace Interpreters {
		using Karkinolution::Binary::Frames::Models::Request::CreateExtra;
		using Karkinolution::Binary::Frames::Models::Request::CycleExtra;
		using Karkinolution::Binary::Frames::Models::Request::EraseExtra;
		using Karkinolution::Binary::Frames::Models::Request::SeeExtra;
		using Karkinolution::Binary::Frames::Models::Request::WantExtra;
		using Karkinolution::Binary::Protocol::Request::SeeTarget;

		inline const ResponseFrame &require_response_frame(const ResponseFrameVariant &frame) {
			if (const auto* response = std::get_if<ResponseFrame>(&frame)) {
				return *response;
			}

			const auto &error = std::get<ErrorFrame>(frame);
			throw ClientError(std::format("Server returned error code {}",
										  static_cast<std::uint8_t>(error.get_sub_type())));
		}
	} // namespace Interpreters
} // namespace KarkinolutionSDK