#pragma once
#include "karkinolution/core/error.hpp"

#include <cstdint>
#include <deque>
#include <karkinolution/binary/binary_validators.hpp>
#include <karkinolution/binary/byte_range.hpp>
#include <karkinolution/binary/byte_utils.hpp>
#include <karkinolution/binary/deserialization/deserializer.hpp>
#include <karkinolution/binary/message_type_size.hpp>
#include <vector>

struct ParsedFrame {
		std::uint32_t                size;
		BinaryTypes                  type;
		BinarySubTypes::CodeSubTypes sub_type;
		std::vector<std::byte>       payload;

		std::optional<RequestResource> resource;

		// this must exist if type == request
		// don't verify directly, use this function below

		bool has_resource() {
			if (type == BinaryTypes::Request) {
				if (!resource.has_value()) {
					throw SimulationError(
						"The resource of a parsed frame must exist if the type is request");
				}
				return true;
			}
			return false;
		}
};

namespace FrameParser {
	std::uint32_t constexpr get_payload_bytes(uint32_t size) {
		return size - static_cast<uint32_t>(MESSAGE_TYPE_BYTES)
			- static_cast<uint32_t>(MESSAGE_SUB_TYPE_BYTES);
	}

	template <ByteRange T> std::uint32_t get_size(const T &bytes) {
		return Deserializer::read_uint32_t(bytes, 0);
	}

	template <ByteRange T> BinaryTypes get_type(const T &bytes) {
		const auto raw = Deserializer::read_uint8_t(bytes, MESSAGE_SIZE_BYTES);

		BinaryValidator::validate_type(raw);
		return static_cast<BinaryTypes>(raw);
	}

	template <ByteRange T> BinarySubTypes::CodeSubTypes get_sub_type(const T &bytes) {
		const auto type = get_type(bytes);
		const auto read =
			Deserializer::read_uint8_t(bytes, MESSAGE_SIZE_BYTES + MESSAGE_TYPE_BYTES);

		BinaryValidator::validate_sub_type(type, read);

		if (type == BinaryTypes::Error) {

			return BinarySubTypes::Error{read};
		} else if (type == BinaryTypes::Request) {
			return BinarySubTypes::Request{read};
		} else if (type == BinaryTypes::Response) {
			return BinarySubTypes::Response{read};
		}
		throw ByteError("Unexpected error");
	}

	template <ByteRange T> std::vector<std::byte> get_payload(const T &bytes) {
		std::vector<std::byte> payload;

		const auto offset = MESSAGE_SIZE_BYTES + MESSAGE_TYPE_BYTES + MESSAGE_SUB_TYPE_BYTES;

		payload.insert(payload.end(), bytes.begin() + offset, bytes.end());

		return payload;
	}

	template <ByteRange T> RequestResource get_resource(const T &bytes) {
		const auto type = get_type(bytes);

		if (type != BinaryTypes::Request) {
			throw ByteError("Only request has a resource type");
		}

		const auto read = Deserializer::read_uint8_t(bytes, FrameOffsets::TO_GET_REQUEST_RESOURCE);

		RequestResource resource;
		if (read == static_cast<uint8_t>(RequestResource::CREATURE)) {
			resource = RequestResource::CREATURE;
		} else if (read == static_cast<uint8_t>(RequestResource::CORPSE)) {
			resource = RequestResource::CORPSE;
		} else if (read == static_cast<uint8_t>(RequestResource::EMBRYO)) {
			resource = RequestResource::EMBRYO;
		} else if (read == static_cast<uint8_t>(RequestResource::SOIL)) {
			resource = RequestResource::SOIL;
		} else if (read == static_cast<uint8_t>(RequestResource::EGG)) {
			resource = RequestResource::EGG;
		} else {
			throw ByteError(std::format("Invalid request resource: ", read));
		}
		return resource;
	}

	template <ByteRange T> std::uint32_t get_frame_size(const T &bytes) {
		return MESSAGE_HEADER_BYTES + get_payload_bytes(bytes);
	}

	template <ByteRange T> std::vector<std::byte> get_frame(const T &bytes) {

		// verify if the bytes have enough size to get size, type and sub type
		if (bytes.size() < MESSAGE_HEADER_BYTES) {
			throw ByteError(
				std::format("The size of the bytes {} is lower than required (not including the "
							"size of payload) to get size, type and sub type {}",
							bytes.size(),
							MESSAGE_HEADER_BYTES));
		}

		const auto size = get_size(bytes);

		if (size <= MESSAGE_TYPE_BYTES + MESSAGE_SUB_TYPE_BYTES) {
			throw ByteError(std::format("The declared size {} is lower than required {}",
										size,
										MESSAGE_TYPE_BYTES + MESSAGE_SUB_TYPE_BYTES));
		}


		const auto payload_size = get_payload_bytes(size);
		const auto frame_size   = MESSAGE_HEADER_BYTES + payload_size;

		if (bytes.size() < frame_size) {
			throw ByteError(
				std::format("The size of bytes {} is lower than the frame size value {}",
							bytes.size(),
							frame_size));
		}

		// verify if the bytes have enough size to get the payload
		if (bytes.size()
			< MESSAGE_SIZE_BYTES + MESSAGE_TYPE_BYTES + MESSAGE_SUB_TYPE_BYTES + payload_size) {
			throw ByteError(
				std::format("The size of bytes {} is lower than required (including "
							"the size of payload) to get payload {}",
							bytes.size(),
							MESSAGE_TYPE_BYTES + MESSAGE_SUB_TYPE_BYTES + payload_size));
		}

		std::vector<std::byte> output;
		output.reserve(frame_size);
		output.insert(output.end(), bytes.begin(), bytes.begin() + frame_size);

		return output;
	}

	template <ByteRange T> ParsedFrame parse_frame(const T &bytes) {
		const auto                     type     = get_type(bytes);
		std::optional<RequestResource> resource = std::nullopt;
		const auto                     sub_type = get_sub_type(bytes);
		if (type == BinaryTypes::Request) {
			resource = get_resource(bytes);
			BinaryValidator::validate_request_sub_type_and_resource(
				ByteUtils::cast_numeric(sub_type),
				ByteUtils::cast_numeric(resource.value()));
		}


		return ParsedFrame{.size     = get_size(bytes),
						   .type     = get_type(bytes),
						   .sub_type = get_sub_type(bytes),
						   .payload  = get_payload(bytes),
						   .resource = resource};
	}


	enum class FrameFieldExistence : uint8_t {
		YES,
		INCOMPLETE,
		NO
	};

	template <ByteRange T> FrameFieldExistence has_size(const T &byte) {
		if (byte.size() == 0) {
			return FrameFieldExistence::NO;
		} else if (byte.size() < MESSAGE_SIZE_BYTES) {
			return FrameFieldExistence::INCOMPLETE;
		} else {
			return FrameFieldExistence::YES;
		}
	}

	template <ByteRange T> FrameFieldExistence has_type(const T &byte) {
		const auto v_has_size = has_size(byte);

		if (v_has_size != FrameFieldExistence::YES) {
			return FrameFieldExistence::NO;
		}

		if (byte.size() == MESSAGE_SIZE_BYTES) {
			return FrameFieldExistence::NO;
		} else if (byte.size() <= MESSAGE_SIZE_BYTES + MESSAGE_TYPE_BYTES) {
			return FrameFieldExistence::INCOMPLETE;
		} else {
			return FrameFieldExistence::YES;
		}
	}

	template <ByteRange T> FrameFieldExistence has_sub_type(const T &byte) {
		const auto v_has_type = has_type(byte);

		if (v_has_type != FrameFieldExistence::YES) {
			return FrameFieldExistence::NO;
		}

		if (byte.size() == MESSAGE_SIZE_BYTES + MESSAGE_TYPE_BYTES) {
			return FrameFieldExistence::NO;
		} else if (byte.size()
				   <= MESSAGE_SIZE_BYTES + MESSAGE_TYPE_BYTES + MESSAGE_SUB_TYPE_BYTES) {
			return FrameFieldExistence::INCOMPLETE;
		} else {
			return FrameFieldExistence::YES;
		}
	}

	template <ByteRange T> FrameFieldExistence has_payload(const T &byte) {
		/*
		This function answers if the sequence of bytes has a complete payload
		Works with Type = Request
		*/


		const auto v_has_sub_type = has_sub_type(byte);


		if (v_has_sub_type != FrameFieldExistence::YES) {
			return FrameFieldExistence::NO;
		}

		const auto size = get_size(byte);

		if (byte.size() == MESSAGE_HEADER_BYTES) {
			return FrameFieldExistence::NO;
		} else if (size < MESSAGE_TYPE_BYTES + MESSAGE_SUB_TYPE_BYTES + get_payload_bytes(size)) {

			return FrameFieldExistence::INCOMPLETE;
		} else {
			return FrameFieldExistence::YES;
		}
	}

	template <ByteRange T> FrameFieldExistence has_request_resource(const T &byte) {
		/*
		This function is used when you need to check if the payload has the bytes for get the
		resource
		*/
		const auto v_has_sub_type = has_sub_type(byte);

		if (v_has_sub_type != FrameFieldExistence::YES) {
			return FrameFieldExistence::NO;
		}

		if (byte.size() == MESSAGE_SIZE_BYTES + MESSAGE_TYPE_BYTES + MESSAGE_SUB_TYPE_BYTES) {
			return FrameFieldExistence::NO;
		} else if (byte.size() <= MESSAGE_SIZE_BYTES + MESSAGE_TYPE_BYTES + MESSAGE_SUB_TYPE_BYTES
					   + REQUEST_RESOURCE_BYTES) {
			return FrameFieldExistence::INCOMPLETE;
		} else {
			return FrameFieldExistence::YES;
		}
	}

	template <ByteRange T> FrameFieldExistence has_frame(const T &bytes) {
		const auto v_has_size     = has_size(bytes);
		const auto v_has_type     = has_type(bytes);
		const auto v_has_sub_type = has_sub_type(bytes);
		const auto v_has_payload  = has_payload(bytes);


		if (v_has_size == FrameFieldExistence::NO && v_has_type == FrameFieldExistence::NO
			&& v_has_sub_type == FrameFieldExistence::NO
			&& v_has_payload == FrameFieldExistence::NO) {
			return FrameFieldExistence::NO;
		}
		if (v_has_size != FrameFieldExistence::YES || v_has_type != FrameFieldExistence::YES
			|| v_has_sub_type != FrameFieldExistence::YES
			|| v_has_payload != FrameFieldExistence::YES) {
			return FrameFieldExistence::INCOMPLETE;
		}
		return FrameFieldExistence::YES;
	}
} // namespace FrameParser
