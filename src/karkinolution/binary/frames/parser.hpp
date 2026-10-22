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
#include <karkinolution/binary/protocol/layouts.hpp>
#include <string>
#include <utility>
#include <vector>

namespace Frames {

	namespace Parser {

		constexpr std::uint32_t calculate_declared_size(std::uint32_t payload_bytes,
														BinaryTypes   type) {
			std::uint32_t common_bytes = ProtocolLayout::Type::SIZE + ProtocolLayout::SubType::SIZE
				+ ProtocolLayout::RequestTypes::Request::Id::SIZE;

			std::uint32_t extra_bytes = common_bytes;

			if (type == BinaryTypes::Request) {
				extra_bytes += ProtocolLayout::RequestTypes::Request::Resource::SIZE;
			}
			return extra_bytes + payload_bytes;
		}

		constexpr std::uint32_t calculate_payload_bytes(std::uint32_t declared_size,
														BinaryTypes   type) {
			constexpr auto common_bytes = ProtocolLayout::Type::SIZE + ProtocolLayout::SubType::SIZE
				+ ProtocolLayout::RequestTypes::Request::Id::SIZE;

			if (declared_size < common_bytes) {
				throw ByteError(
					std::format("Cannot get payload bytes because size {} < common bytes {}",
								declared_size,
								common_bytes));
			}
			if (type == BinaryTypes::Request) {
				const auto minimum_bytes =
					common_bytes + ProtocolLayout::RequestTypes::Request::Resource::SIZE;

				if (declared_size < minimum_bytes) {
					throw ByteError(
						std::format("Cannot get payload bytes because size {} < minimum bytes {}",
									declared_size,
									minimum_bytes));
				}
				return declared_size - common_bytes
					- ProtocolLayout::RequestTypes::Request::Resource::SIZE;
			} else {
				return declared_size - common_bytes;
			}
		}

		template <ByteRange T>
		constexpr std::uint32_t calculate_payload_bytes(const T &bytes, BinaryTypes type) {
			const auto declared_size = get_declared_size(bytes);

			return calculate_payload_bytes(declared_size, type);
		}

		template <ByteRange T> bool is_complete_frame(const T &bytes) {
			return bytes.size() == get_frame_size(bytes);
		}

		template <ByteRange T> std::uint32_t get_declared_size(const T &bytes) {
			return Deserializer::read_uint32_t(bytes, 0);
		}

		template <ByteRange T> BinaryTypes get_type(const T &bytes) {
			const auto raw = Deserializer::read_uint8_t(bytes, ProtocolLayout::DeclaredSize::SIZE);

			BinaryValidator::validate_type(raw);
			return static_cast<BinaryTypes>(raw);
		}

		template <ByteRange T>
		Karkinolution::Binary::Protocol::Types::BinarySubTypes::CodeSubTypes
		get_sub_type(const T &bytes) {
			using namespace Karkinolution::Binary::Protocol::Types::BinarySubTypes;
			const auto type = get_type(bytes);
			const auto read = Deserializer::read_uint8_t(bytes, ProtocolLayout::SubType::OFFSET);

			BinaryValidator::validate_sub_type(type, read);

			if (type == BinaryTypes::Error) {
				return Error{read};
			} else if (type == BinaryTypes::Request) {
				return Request{read};
			} else if (type == BinaryTypes::Response) {
				return Response{read};
			}
			throw ByteError("Unexpected error");
		}

		namespace Request {
			template <ByteRange T> RequestId get_id(const T &bytes) {
				return Deserializer::deserialize<RequestId>(
					bytes,
					ProtocolLayout::RequestTypes::Request::Id::OFFSET);
			}

			template <ByteRange T> RequestResource get_resource(const T &bytes) {
				auto read = Deserializer::deserialize<RequestResourceEnumType>(
					bytes,
					ProtocolLayout::RequestTypes::Request::Resource::OFFSET);

				BinaryValidator::Request::validate_resource(read);
				return RequestResource(read);
			}

			template <ByteRange T> std::vector<std::byte> get_payload(const T &bytes) {
				std::vector<std::byte> payload;
				payload.insert(payload.end(),
							   bytes.begin()
								   + ProtocolLayout::RequestTypes::Request::Payload::OFFSET,
							   bytes.end());
				return payload;
			}
		} // namespace Request

		namespace Response {
			template <ByteRange T> RequestId get_id(const T &bytes) {
				return Deserializer::deserialize<RequestId>(
					bytes,
					ProtocolLayout::ResponseTypes::RequestId::OFFSET);
			}

			template <ByteRange T> std::vector<std::byte> get_payload(const T &bytes) {
				std::vector<std::byte> payload;
				payload.insert(payload.end(),
							   bytes.begin() + ProtocolLayout::ResponseTypes::Payload::OFFSET,
							   bytes.end());
				return payload;
			}
		} // namespace Response

		template <ByteRange T> std::vector<std::byte> get_payload(const T &bytes) {
			const auto             type = get_type(bytes);
			std::vector<std::byte> payload;

			if (type == BinaryTypes::Request) {
				return Request::get_payload(bytes);
			} else if (type == BinaryTypes::Response || type == BinaryTypes::Error) {
				return Response::get_payload(bytes);
			}

			return payload;
			std::unreachable();
		}

		template <ByteRange T> std::uint32_t get_frame_size(const T &bytes) {
			return ProtocolLayout::DeclaredSize::SIZE + get_declared_size(bytes);
		}

		template <ByteRange T> std::vector<std::byte> get_frame(const T &bytes) {
			// verify if the bytes have enough size to get size, type and sub type
			if (bytes.size() < ProtocolLayout::TOTAL_HEADER_SIZE) {
				throw ByteError(std::format(
					"The size of the bytes {} is lower than required (not including the "
					"size of payload) to get size, type and sub type {}",
					bytes.size(),
					ProtocolLayout::TOTAL_HEADER_SIZE));
			}

			const auto declared_size = get_declared_size(bytes);

			if (declared_size <= ProtocolLayout::Type::SIZE + ProtocolLayout::SubType::SIZE) {
				throw ByteError(
					std::format("The declared size {} is lower than required {}",
								declared_size,
								ProtocolLayout::Type::SIZE + ProtocolLayout::SubType::SIZE));
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
			if (bytes.size() < ProtocolLayout::DeclaredSize::SIZE + ProtocolLayout::Type::SIZE
					+ ProtocolLayout::SubType::SIZE + payload_size) {
				throw ByteError(std::format(
					"The size of bytes {} is lower than required (including "
					"the size of payload) to get payload {}",
					bytes.size(),
					ProtocolLayout::Type::SIZE + ProtocolLayout::SubType::SIZE + payload_size));
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
			if (type == BinaryTypes::Request) {
				return Models::GenericFrame{parse_request_frame(bytes), type};
			} else if (type == BinaryTypes::Response || type == BinaryTypes::Error) {
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
			} else if (byte.size() < ProtocolLayout::DeclaredSize::SIZE) {
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

			if (byte.size() == ProtocolLayout::DeclaredSize::SIZE) {
				return FrameFieldExistence::NO;
			} else if (byte.size()
					   < ProtocolLayout::DeclaredSize::SIZE + ProtocolLayout::Type::SIZE) {
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

			if (byte.size() == ProtocolLayout::DeclaredSize::SIZE + ProtocolLayout::Type::SIZE) {
				return FrameFieldExistence::NO;
			} else if (byte.size() < ProtocolLayout::DeclaredSize::SIZE + ProtocolLayout::Type::SIZE
						   + ProtocolLayout::SubType::SIZE) {
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

			const auto frame_size = ProtocolLayout::DeclaredSize::SIZE + declared_size;

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
