#pragma once
#include "karkinolution/binary/frames/aliases.hpp"

#include <bit>
#include <vector>

namespace Karkinolution {
	namespace Binary {
		namespace Frames {
			namespace Analyzer {
				bool is_error(const std::vector<std::byte> &frame);
				bool is_response(const std::vector<std::byte> &frame);
				bool is_request(const std::vector<std::byte> &frame);


				bool is_none(const std::vector<std::byte> &frame);
				bool is_none(const ResponseFrame &frame);

				template <ByteRange T> bool is_complete_frame(const T &bytes) {
					return bytes.size() == get_frame_size(bytes);
				}
			} // namespace Analyzer
		} // namespace Frames
	} // namespace Binary
} // namespace Karkinolution
