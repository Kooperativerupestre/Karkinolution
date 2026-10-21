#pragma once
#include "karkinolution/binary/core_aliases.hpp"
#include "karkinolution/binary/frames/invariants.hpp"
#include "karkinolution/core/error.hpp"

#include <cstdint>
#include <deque>
#include <functional>
#include <karkinolution/binary/binary_validators.hpp>
#include <karkinolution/binary/byte_range.hpp>
#include <karkinolution/binary/byte_utils.hpp>
#include <karkinolution/binary/deserializer.hpp>
#include <karkinolution/binary/frames/models.hpp>
#include <karkinolution/binary/message_type_size.hpp>
#include <string>
#include <utility>
#include <vector>

namespace Frames {

	namespace Parser {
		constexpr std::uint32_t calculate_declared_size(std::uint32_t              payload_bytes,
														Frames::Types::BinaryTypes type) {
			std::uint32_t common_bytes = Frames::FieldsBytes::TYPE + Frames::FieldsBytes::SUB_TYPE
				+ Frames::FieldsBytes::RequestTypes::Request::ID;


			std::uint32_t extra_bytes = common_bytes;

			if (type == BinaryTypes::Request) {
				extra_bytes += Frames::FieldsBytes::RequestTypes::Request::RESOURCE;
			}
			return extra_bytes + payload_bytes;
		}

		constexpr std::uint32_t calculate_payload_bytes(std::uint32_t              declared_size,
														Frames::Types::BinaryTypes type) {
			constexpr auto common_bytes = Frames::FieldsBytes::TYPE + Frames::FieldsBytes::SUB_TYPE
				+ Frames::FieldsBytes::RequestTypes::Request::ID;


			if (declared_size < common_bytes) {
				throw ByteError(
					std::format("Cannot get payload bytes because size {} < common bytes {}",
								declared_size,
								common_bytes));
			}
			if (type == Frames::Types::BinaryTypes::Request) {
				const auto minimum_bytes =
					common_bytes + Frames::FieldsBytes::RequestTypes::Request::RESOURCE;

				if (declared_size < minimum_bytes) {
					throw ByteError(
						std::format("Cannot get payload bytes because size {} < common bytes {}",
									declared_size,
									common_bytes));
				}
				return declared_size - common_bytes
					- Frames::FieldsBytes::RequestTypes::Request::RESOURCE;
			}

			if (type == Frames::Types::BinaryTypes::Error
				|| type == Frames::Types::BinaryTypes::Response) {
				if (declared_size < common_bytes) {
					throw ByteError(
						std::format("Cannot get payload bytes because size {} < common bytes {}",
									declared_size,
									common_bytes));
				}
				return declared_size - common_bytes;
			}

			throw ByteError("Invalid type of frame");
		}

		template <ByteRange T> std::uint32_t get_declared_size(const T &bytes) {
			return Deserializer::read_uint32_t(bytes, 0);
		}

		template <ByteRange T> Frames::Types::BinaryTypes get_type(const T &bytes) {
			const auto raw = Deserializer::read_uint8_t(bytes, Frames::FieldsBytes::DECLARED_SIZE);

			BinaryValidator::validate_type(raw);
			return static_cast<Frames::Types::BinaryTypes>(raw);
		}

		template <ByteRange T>
		Frames::Types::BinarySubTypes::CodeSubTypes get_sub_type(const T &bytes) {
			const auto type = get_type(bytes);
			const auto read = Deserializer::read_uint8_t(bytes, Frames::Offsets::TO_GET_SUB_TYPE);

			BinaryValidator::validate_sub_type(type, read);

			if (type == Frames::Types::BinaryTypes::Error) {

				return Frames::Types::BinarySubTypes::Error{read};
			} else if (type == Frames::Types::BinaryTypes::Request) {
				return Frames::Types::BinarySubTypes::Request{read};
			} else if (type == Frames::Types::BinaryTypes::Response) {
				return Frames::Types::BinarySubTypes::Response{read};
			}
			throw ByteError("Unexpected error");
		}

		namespace Request {
			template <ByteRange T> Frames::Types::RequestId get_id(const T &bytes) {
				return Deserializer::deserialize<Frames::Types::RequestId>(
					bytes,
					Frames::Offsets::RequestTypes::Request::TO_GET_ID);
			}

			template <ByteRange T> Frames::Types::RequestResource get_resource(const T &bytes) {
				auto read = Deserializer::deserialize<Frames::Types::RequestResourceEnumType>(
					bytes,
					Frames::Offsets::RequestTypes::Request::TO_GET_RESOURCE);

				BinaryValidator::Request::validate_resource(read);
				return Frames::Types::RequestResource(read);
			}

			template <ByteRange T> std::vector<std::byte> get_payload(const T &bytes) {
				std::vector<std::byte> payload;
				payload.insert(payload.end(),
							   bytes.begin()
								   + Frames::Offsets::RequestTypes::Request::TO_GET_PAYLOAD,
							   bytes.end());
				return payload;
			}
		} // namespace Request

		namespace Response {
			template <ByteRange T> Frames::Types::RequestId get_id(const T &bytes) {
				return Deserializer::deserialize<Frames::Types::RequestId>(
					bytes,
					Frames::Offsets::ResponseTypes::TO_GET_REQUEST_ID);
			}

			template <ByteRange T> std::vector<std::byte> get_payload(const T &bytes) {
				std::vector<std::byte> payload;
				payload.insert(payload.end(),
							   bytes.begin() + Frames::Offsets::ResponseTypes::TO_GET_PAYLOAD,
							   bytes.end());
				return payload;
			}
		} // namespace Response

		template <ByteRange T> std::vector<std::byte> get_payload(const T &bytes) {
			const auto             type = get_type(bytes);
			std::vector<std::byte> payload;


			if (type == Frames::Types::BinaryTypes::Request) {
				payload = Request::get_payload(bytes);
			} else if (type == Frames::Types::BinaryTypes::Error
					   || type == Frames::Types::BinaryTypes::Response) {
				payload = Response::get_payload(bytes);
			} else {
				throw ByteError("Invalid frame type");
			}
			return payload;
			std::unreachable();
		}

		template <ByteRange T> std::uint32_t get_frame_size(const T &bytes) {
			return Frames::FieldsBytes::DECLARED_SIZE + get_declared_size(bytes);
		}

		template <ByteRange T> std::vector<std::byte> get_frame(const T &bytes) {

			// verify if the bytes have enough size to get size, type and sub type
			if (bytes.size() < Frames::FieldsBytes::MESSAGE_HEADER_BYTES) {
				throw ByteError(std::format(
					"The size of the bytes {} is lower than required (not including the "
					"size of payload) to get size, type and sub type {}",
					bytes.size(),
					Frames::FieldsBytes::MESSAGE_HEADER_BYTES));
			}

			const auto declared_size = get_declared_size(bytes);

			if (declared_size <= Frames::FieldsBytes::TYPE + Frames::FieldsBytes::SUB_TYPE) {
				throw ByteError(
					std::format("The declared size {} is lower than required {}",
								declared_size,
								Frames::FieldsBytes::TYPE + Frames::FieldsBytes::SUB_TYPE));
			}

			const auto type = get_type(bytes);

			const auto payload_size = calculate_payload_bytes(declared_size, type);
			const auto frame_size   = get_frame_size(bytes);

			if (bytes.size() < frame_size) {
				throw ByteError(
					std::format("The size of bytes {} is lower than the frame size value {}",
								bytes.size(),
								frame_size));
			}

			// verify if the bytes have enough size to get the payload
			if (bytes.size() < Frames::FieldsBytes::DECLARED_SIZE + Frames::FieldsBytes::TYPE
					+ Frames::FieldsBytes::SUB_TYPE + payload_size) {
				throw ByteError(std::format(
					"The size of bytes {} is lower than required (including "
					"the size of payload) to get payload {}",
					bytes.size(),
					Frames::FieldsBytes::TYPE + Frames::FieldsBytes::SUB_TYPE + payload_size));
			}

			std::vector<std::byte> output;
			output.reserve(frame_size);
			output.insert(output.end(), bytes.begin(), bytes.begin() + frame_size);

			return output;
		}

		template <ByteRange T> Models::Request parse_request_frame(const T &bytes) {
			const auto sub_type = std::get<RequestSubTypes>(get_sub_type(bytes));
			const auto resource = Request::get_resource(bytes);

			BinaryValidator::Request::validate_request_sub_type_and_resource(
				BinaryUtils::cast_numeric(sub_type),
				BinaryUtils::cast_numeric(resource));

			return Models::Request{.size     = get_declared_size(bytes),
								   .id       = Request::get_id(bytes),
								   .sub_type = sub_type,
								   .payload  = get_payload(bytes),
								   .resource = resource};
		}

		template <ByteRange T> Models::ResponseGeneric parse_response_frame(const T &bytes) {

			return Models::ResponseGeneric{.size       = get_declared_size(bytes),
										   .request_id = Response::get_id(bytes),
										   .sub_type   = get_sub_type(bytes),
										   .payload    = Response::get_payload(bytes)};
		}

		template <ByteRange T> Models::GenericFrame parse_frame(const T &bytes) {
			const auto type = get_type(bytes);
			BinaryValidator::validate_type(type);
			if (type
				== static_cast<Frames::Types::FrameType>(Frames::Types::BinaryTypes::Request)) {
				return Models::GenericFrame{parse_request_frame(bytes), type};
			} else if (type
						   == static_cast<Frames::Types::FrameType>(
							   Frames::Types::BinaryTypes::Response)
					   || type
						   == static_cast<Frames::Types::FrameType>(
							   Frames::Types::BinaryTypes::Error)) {
				return Models::GenericFrame{parse_response_frame(bytes), type};
			}
			std::unreachable();
		}


		enum class FrameFieldExistence : uint8_t {
			YES,
			INCOMPLETE,
			NO
		};

		template <ByteRange T> FrameFieldExistence has_size(const T &byte) {
			if (byte.size() == 0) {
				return FrameFieldExistence::NO;
			} else if (byte.size() < Frames::FieldsBytes::DECLARED_SIZE) {
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

			if (byte.size() == Frames::FieldsBytes::DECLARED_SIZE) {
				return FrameFieldExistence::NO;
			} else if (byte.size()
					   < Frames::FieldsBytes::DECLARED_SIZE + Frames::FieldsBytes::TYPE) {
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

			if (byte.size() == Frames::FieldsBytes::DECLARED_SIZE + Frames::FieldsBytes::TYPE) {
				return FrameFieldExistence::NO;
			} else if (byte.size() < Frames::FieldsBytes::DECLARED_SIZE + Frames::FieldsBytes::TYPE
						   + Frames::FieldsBytes::SUB_TYPE) {
				return FrameFieldExistence::INCOMPLETE;
			} else {
				return FrameFieldExistence::YES;
			}
		}

		template <ByteRange T> FrameFieldExistence has_payload(const T &byte) {

			const auto v_has_sub_type = has_sub_type(byte);

			if (v_has_sub_type != FrameFieldExistence::YES) {
				return FrameFieldExistence::NO;
			}

			const auto declared_size = get_declared_size(byte);

			const auto frame_size = Frames::FieldsBytes::DECLARED_SIZE + declared_size;

			FramesInvariants::payload_size_cannot_be_zero(declared_size, byte.size());

			if (byte.size() < frame_size) {
				return FrameFieldExistence::INCOMPLETE;
			}

			return FrameFieldExistence::YES;
		}

		template <ByteRange T> FrameFieldExistence has_frame(const T &bytes) {
			const auto v_has_size = has_size(bytes);

			if (v_has_size == FrameFieldExistence::NO) {
				return FrameFieldExistence::NO;
			} else if (v_has_size == FrameFieldExistence::INCOMPLETE) {
				return FrameFieldExistence::INCOMPLETE;
			}


			const auto v_has_type = has_type(bytes);

			if (v_has_type != FrameFieldExistence::YES) {
				return FrameFieldExistence::INCOMPLETE;
			}

			const auto v_has_sub_type = has_sub_type(bytes);

			if (v_has_sub_type != FrameFieldExistence::YES) {
				return FrameFieldExistence::INCOMPLETE;
			}

			const auto v_has_payload = has_payload(bytes);

			if (v_has_payload != FrameFieldExistence::YES) {
				return FrameFieldExistence::INCOMPLETE;
			}


			return FrameFieldExistence::YES;
		}
	} // namespace Parser
} // namespace Frames
