#include "karkinolution/networking/raw_frame_buffer.hpp"

BufferExistence FrameBufferAnalyzer::has_frame(const RawFrameBuffer &buffer) {
	return Karkinolution::Binary::Frames::Parser::has_frame(buffer.view());
}

std::optional<std::vector<std::byte>>
FrameBufferAnalyzer::get_frame(const RawFrameBuffer &buffer) {
	if (has_frame(buffer) != BufferExistence::YES) {
		return std::nullopt;
	}

	return Karkinolution::Binary::Frames::Parser::get_frame(buffer.view());
}
