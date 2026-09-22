#include "karkinolution/binary/message_type_size.hpp"

#include <karkinolution/binary/frames/parser.hpp>
#include <karkinolution/core/error.hpp>
#include <karkinolution/networking/buffer.hpp>

BufferExistence FrameBufferAnalyzer::has_size(const RequestBuffer &buffer) {
	const auto &view = buffer.view();

	if (view.empty()) {
		return BufferExistence::NO;
	} else if (view.size() < MESSAGE_SIZE_BYTES) {
		return BufferExistence::INCOMPLETE;
	} else {
		return BufferExistence::YES;
	}
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

	if (view.size() <= MESSAGE_SIZE_BYTES + MESSAGE_TYPE_BYTES) {
		return BufferExistence::NO;
	} else if (view.size() < MESSAGE_SIZE_BYTES + MESSAGE_TYPE_BYTES + MESSAGE_SUB_TYPE_BYTES) {
		return BufferExistence::INCOMPLETE;
	} else {
		return BufferExistence::YES;
	}
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

	if (view.size() <= MESSAGE_SIZE_BYTES) {
		return BufferExistence::NO;
	} else if (view.size() < MESSAGE_SIZE_BYTES + MESSAGE_TYPE_BYTES) {
		return BufferExistence::INCOMPLETE;
	} else {
		return BufferExistence::YES;
	}
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

	const auto has_size_output = has_size(buffer);
	if (has_size_output != BufferExistence::YES) {
		return has_size_output;
	}

	const auto frame_size   = MESSAGE_SIZE_BYTES + get_size(buffer).value();
	const auto payload_size = frame_size - MESSAGE_HEADER_BYTES;

	if (payload_size > MAX_PAYLOAD_BYTES) {
		throw BufferError("The size of the frame payload is bigger than allowed");
	}

	if (view.size() < frame_size) {
		return BufferExistence::INCOMPLETE;
	}

	return BufferExistence::YES;
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
	const auto has_code_output    = has_sub_type(buffer);
	const auto has_payload_output = has_payload(buffer);
	const auto has_type_output    = has_type(buffer);
	const auto has_size_output    = has_size(buffer);

	if (has_code_output == BufferExistence::YES && has_payload_output == BufferExistence::YES
		&& has_type_output == BufferExistence::YES && has_size_output == BufferExistence::YES) {
		return BufferExistence::YES;
	} else if (has_code_output == BufferExistence::NO && has_payload_output == BufferExistence::NO
			   && has_type_output == BufferExistence::NO
			   && has_size_output == BufferExistence::NO) {
		return BufferExistence::NO;
	} else {
		return BufferExistence::INCOMPLETE;
	}
}

std::optional<std::vector<std::byte>> FrameBufferAnalyzer::get_frame(RequestBuffer &buffer) {
	const auto has_frame_output = has_frame(buffer);

	if (has_frame_output != BufferExistence::YES) {
		return std::nullopt;
	}

	const auto frame_size = MESSAGE_SIZE_BYTES + get_size(buffer).value();

	std::vector<std::byte> frame;
	frame.reserve(frame_size);

	for (size_t i = 0; i < frame_size; i++) {
		frame.push_back(buffer.pop_front().value());
	}

	return frame;
}
