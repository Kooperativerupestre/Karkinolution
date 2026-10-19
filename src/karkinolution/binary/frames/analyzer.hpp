#pragma once
#include "karkinolution/binary/frames/aliases.hpp"

#include <bit>
#include <vector>

namespace Frames {
	namespace Analyzer {
		bool is_error(const std::vector<std::byte> &frame);
		bool is_response(const std::vector<std::byte> &frame);
		bool is_request(const std::vector<std::byte> &frame);


		bool is_none(const std::vector<std::byte> &frame);
		bool is_none(const ResponseFrame &frame);
	} // namespace Analyzer
} // namespace Frames