#pragma once


#include "karkinolution/binary/byte_range.hpp"
#include "karkinolution/binary/byte_utils.hpp"
#include "karkinolution/binary/frames/parser.hpp"
#include "karkinolution/binary/frames/processor.hpp"
#include "karkinolution/core/error.hpp"

#include <array>
#include <karkinolution/binary/core_aliases.hpp>
#include <karkinolution/binary/deserializer.hpp>
#include <karkinolution/binary/frames/aliases.hpp>
#include <karkinolution/binary/message_type_size.hpp>
#include <karkinolution/binary/serializer.hpp>
#include <vector>

namespace Frames {
	namespace Motor {

		void constexpr append_declared_size(std::vector<std::byte>          &frame,
											Frames::Types::FrameDeclaredSize declared_size) {
			BinaryUtils::append_bytes(
				frame,
				Serializer::serialize<Frames::Types::FrameDeclaredSize>(declared_size));
		}

		void constexpr append_type(std::vector<std::byte> &frame, Frames::Types::BinaryTypes type) {
			BinaryUtils::append_bytes(frame,
									  Serializer::serialize<Frames::Types::FrameType>(
										  static_cast<Frames::Types::FrameType>(type)));
		}

		ResponseFrame make_none_response(Frames::Types::RequestId request_id);

		template <ByteRange Payload>
		ResponseFrame make_response(BinaryTypes              type,
									GenericResponseSubTypes  sub_type,
									Frames::Types::RequestId request_id,
									const Payload           &payload) {
			return ResponseFrame{Frames::Parser::calculate_declared_size(payload.size(), type),
								 request_id,
								 sub_type,
								 payload,
								 type};
		}

		template <ByteRange Payload>
		RequestFrame make_request(RequestSubTypes          sub_type,
								  Frames::Types::RequestId id,
								  RequestResource          resource,
								  const Payload           &payload) {
			return RequestFrame{
				Frames::Parser::calculate_declared_size(payload.size(), BinaryTypes::Request),
				id,
				sub_type,
				payload,
				resource};
		}

		template <ByteRange Payload>
		std::vector<std::byte>
		make_response_raw_bytes(Frames::Types::BinaryTypes                    type,
								std::variant<ErrorSubTypes, ResponseSubTypes> sub_type,
								Frames::Types::RequestId                      request_id,
								const Payload                                &payload) {
			if (type == Frames::Types::BinaryTypes::Response
				&& !std::holds_alternative<Frames::Types::BinarySubTypes::Response>(sub_type)) {
				throw FrameInvariantError(std::format(
					"Sub type of a frame with type Response should be Response sub type"));
			} else if (type == Frames::Types::BinaryTypes::Error
					   && !std::holds_alternative<Frames::Types::BinarySubTypes::Error>(sub_type)) {
				throw FrameInvariantError(
					std::format("Sub type of a frame with type Error should be Error sub type"));
			}


			std::vector<std::byte> frame;

			const Frames::Types::FrameDeclaredSize declared_size = Frames::FieldsBytes::SUB_TYPE
				+ Frames::FieldsBytes::TYPE + payload.size()
				+ Frames::FieldsBytes::RequestTypes::Request::ID;

			frame.reserve(declared_size + Frames::FieldsBytes::DECLARED_SIZE);

			// declared size

			append_declared_size(frame, declared_size);

			// type

			append_type(frame, type);

			// sub type

			Frames::Types::BinarySubTypesEnumType v_sub_type;

			if (std::holds_alternative<Frames::Types::BinarySubTypes::Response>(sub_type)) {
				v_sub_type = static_cast<Frames::Types::BinaryTypesEnumType>(
					std::get<Frames::Types::BinarySubTypes::Response>(sub_type));
			} else if (std::holds_alternative<Frames::Types::BinarySubTypes::Error>(sub_type)) {
				v_sub_type = static_cast<Frames::Types::BinaryTypesEnumType>(
					std::get<Frames::Types::BinarySubTypes::Error>(sub_type));
			}


			BinaryUtils::append_bytes(frame, static_cast<std::byte>(v_sub_type));

			// request id


			BinaryUtils::append_bytes(frame,
									  Serializer::serialize<Frames::Types::RequestId>(request_id));
			// payload
			frame.insert(frame.end(), payload.begin(), payload.end());
			return frame;
		}

		template <ByteRange Payload>
		std::vector<std::byte>
		make_request_raw_bytes(Frames::Types::BinarySubTypes::Request sub_type,
							   Frames::Types::RequestId               id,
							   Frames::Types::RequestResource         resource,
							   const Payload                         &payload) {
			std::vector<std::byte>                 frame;
			const Frames::Types::FrameDeclaredSize declared_size = Frames::FieldsBytes::SUB_TYPE
				+ Frames::FieldsBytes::TYPE + payload.size()
				+ Frames::FieldsBytes::RequestTypes::Request::RESOURCE
				+ Frames::FieldsBytes::RequestTypes::Request::ID;


			frame.reserve(declared_size + Frames::FieldsBytes::DECLARED_SIZE);

			// declared size
			append_declared_size(frame, declared_size);

			// type
			append_type(frame, Frames::Types::BinaryTypes::Request);

			// sub type

			BinaryUtils::append_bytes(
				frame,
				Serializer::serialize<Frames::Types::BinaryTypesEnumType>(
					static_cast<Frames::Types::BinaryTypesEnumType>(sub_type)));


			// id

			BinaryUtils::append_bytes(frame, Serializer::serialize<Frames::Types::RequestId>(id));

			// resource
			BinaryUtils::append_bytes(
				frame,
				Serializer::serialize<Frames::Types::RequestResourceEnumType>(
					static_cast<Frames::Types::RequestResourceEnumType>(resource)));
			// payload
			frame.insert(frame.end(), payload.begin(), payload.end());
			return frame;
		}

	} // namespace Motor
} // namespace Frames
