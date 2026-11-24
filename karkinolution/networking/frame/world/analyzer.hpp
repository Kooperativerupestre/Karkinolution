#pragma once
#include "karkinolution/networking/frame/world/aliases.hpp"
#include "karkinolution/networking/frame/world/parser/parser.hpp"

#include <bit>
#include <vector>

namespace Karkinolution {
	namespace Networking {
		namespace Frames::World {
			namespace Analyzer {
				bool is_error(const std::vector<std::byte> &frame);
				bool is_response(const std::vector<std::byte> &frame);
				bool is_request(const std::vector<std::byte> &frame);


				bool is_none(const std::vector<std::byte> &frame);
				bool is_none(const ResponseFrame &frame);

				template <KShared::Binary::ByteRange T> bool is_complete_frame(const T &bytes) {
					return bytes.size() == Parser::get_frame_size(bytes);
				}
			} // namespace Analyzer
		} // namespace Frames::World
	} // namespace Networking
} // namespace Karkinolution
