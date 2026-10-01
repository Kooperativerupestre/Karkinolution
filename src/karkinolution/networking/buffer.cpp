#include "buffer.hpp"

#include "karkinolution/binary/message_type_size.hpp"

#include <iomanip>
#include <iostream>
#include <karkinolution/binary/frames/parser.hpp>
#include <karkinolution/core/error.hpp>
#include <karkinolution/networking/buffer.hpp>

BufferExistence FrameBufferAnalyzer::has_size(const RequestBuffer &buffer) {
	const auto &view = buffer.view();
	return FrameParser::has_size(view);
}

std::optional<std::size_t> FrameBufferAnalyzer::get_size(const RequestBuffer &buffer) {
	const auto &view = buffer.view();

	const auto has_size_output = has_size(buffer);

	if (has_size_output != BufferExistence::YES) {
		return std::nullopt;
	}
	return FrameParser::get_size(view);
}

BufferExistence FrameBufferAnalyzer::has_sub_type(const RequestBuffer &buffer) {
	const auto &view = buffer.view();
	return FrameParser::has_sub_type(view);
}

std::optional<BinarySubTypes::CodeSubTypes>
FrameBufferAnalyzer::get_sub_type(const RequestBuffer &buffer) {
	const auto &view = buffer.view();

	const auto has_code_output = has_sub_type(buffer);

	if (has_code_output != BufferExistence::YES) {
		return std::nullopt;
	}
	return FrameParser::get_sub_type(view);
}

BufferExistence FrameBufferAnalyzer::has_type(const RequestBuffer &buffer) {
	const auto &view = buffer.view();
	return FrameParser::has_type(view);
}

std::optional<BinaryTypes> FrameBufferAnalyzer::get_type(const RequestBuffer &buffer) {
	const auto &view = buffer.view();

	const auto has_type_output = has_type(buffer);

	if (has_type_output != BufferExistence::YES) {
		return std::nullopt;
	}
	return FrameParser::get_type(view);
}

BufferExistence FrameBufferAnalyzer::has_payload(const RequestBuffer &buffer) {
	const auto &view = buffer.view();
	return FrameParser::has_payload(view);
}

std::optional<std::vector<std::byte>>
FrameBufferAnalyzer::get_payload(const RequestBuffer &buffer) {
	const auto &view = buffer.view();

	const auto has_payload_output = has_payload(buffer);

	if (has_payload_output != BufferExistence::YES) {
		return std::nullopt;
	}
	return FrameParser::get_payload(view);
}

BufferExistence FrameBufferAnalyzer::has_frame(const RequestBuffer &buffer) {
	const auto &view = buffer.view();
	return FrameParser::has_frame(view);
}

std::optional<std::vector<std::byte>> FrameBufferAnalyzer::get_frame(RequestBuffer &buffer) {
	const auto has_frame_output = has_frame(buffer);

	if (has_frame_output != BufferExistence::YES) {
		return std::nullopt;
	}

	return FrameParser::get_frame(buffer.view());
}
