#include "karkinolution/binary/message_type_size.hpp"
#include "karkinolution/networking/raw_frame_buffer.hpp"

#include <iomanip>
#include <iostream>
#include <karkinolution/binary/frames/parser.hpp>
#include <karkinolution/core/error.hpp>

BufferExistence FrameBufferAnalyzer::has_size(const RawFrameBuffer &buffer) {
	const auto &view = buffer.view();
	return Frames::Parser::has_size(view);
}

std::optional<std::size_t> FrameBufferAnalyzer::get_size(const RawFrameBuffer &buffer) {
	const auto &view = buffer.view();

	const auto has_size_output = has_size(buffer);

	if (has_size_output != BufferExistence::YES) {
		return std::nullopt;
	}
	return Frames::Parser::get_declared_size(view);
}

BufferExistence FrameBufferAnalyzer::has_sub_type(const RawFrameBuffer &buffer) {
	const auto &view = buffer.view();
	return Frames::Parser::has_sub_type(view);
}

std::optional<CodeSubTypes> FrameBufferAnalyzer::get_sub_type(const RawFrameBuffer &buffer) {
	const auto &view = buffer.view();

	const auto has_code_output = has_sub_type(buffer);

	if (has_code_output != BufferExistence::YES) {
		return std::nullopt;
	}
	return Frames::Parser::get_sub_type(view);
}

BufferExistence FrameBufferAnalyzer::has_type(const RawFrameBuffer &buffer) {
	const auto &view = buffer.view();
	return Frames::Parser::has_type(view);
}

std::optional<BinaryTypes> FrameBufferAnalyzer::get_type(const RawFrameBuffer &buffer) {
	const auto &view = buffer.view();

	const auto has_type_output = has_type(buffer);

	if (has_type_output != BufferExistence::YES) {
		return std::nullopt;
	}
	return Frames::Parser::get_type(view);
}

BufferExistence FrameBufferAnalyzer::has_payload(const RawFrameBuffer &buffer) {
	const auto &view = buffer.view();
	return Frames::Parser::has_payload(view);
}

std::optional<std::vector<std::byte>>
FrameBufferAnalyzer::get_payload(const RawFrameBuffer &buffer) {
	const auto &view = buffer.view();

	const auto has_payload_output = has_payload(buffer);

	if (has_payload_output != BufferExistence::YES) {
		return std::nullopt;
	}
	return Frames::Parser::get_payload(view);
}

BufferExistence FrameBufferAnalyzer::has_frame(const RawFrameBuffer &buffer) {
	const auto &view = buffer.view();
	return Frames::Parser::has_frame(view);
}

std::optional<std::vector<std::byte>> FrameBufferAnalyzer::get_frame(RawFrameBuffer &buffer) {
	const auto has_frame_output = has_frame(buffer);

	if (has_frame_output != BufferExistence::YES) {
		return std::nullopt;
	}

	return Frames::Parser::get_frame(buffer.view());
}
