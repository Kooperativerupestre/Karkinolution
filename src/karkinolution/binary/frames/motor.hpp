#pragma once


#include "karkinolution/binary/byte_range.hpp"
#include "karkinolution/binary/byte_utils.hpp"
#include "karkinolution/binary/frames/parser.hpp"
#include "karkinolution/binary/frames/processor.hpp"
#include "karkinolution/binary/protocol/aliases.hpp"
#include "karkinolution/binary/protocol/contract.hpp"
#include "karkinolution/core/error.hpp"

#include <array>
#include <karkinolution/binary/core_aliases.hpp>
#include <karkinolution/binary/deserializer.hpp>
#include <karkinolution/binary/frames/aliases.hpp>
#include <karkinolution/binary/protocol/layouts.hpp>
#include <karkinolution/binary/serializer.hpp>
#include <vector>

namespace Karkinolution {
	namespace Binary {
		namespace Frames {
			namespace Motor {
				void constexpr append_declared_size(
					std::vector<std::byte>              &frame,
					Protocol::Aliases::FrameDeclaredSize declared_size) {
					Karkinolution::Binary::Utils::append_bytes(
						frame,
						Serializer::serialize<Protocol::Aliases::FrameDeclaredSize>(declared_size));
				}

				void constexpr append_type(std::vector<std::byte> &frame, BinaryTypes type) {
					Karkinolution::Binary::Utils::append_bytes(
						frame,
						Serializer::serialize<Karkinolution::Binary::Protocol::Aliases::FrameType>(
							static_cast<Karkinolution::Binary::Protocol::Aliases::FrameType>(
								type)));
				}

				ResponseFrame make_none_response(RequestId request_id);

				template <ByteRange Payload>
				ResponseFrame
				make_response(BinaryTypes                                              type,
							  Karkinolution::Binary::Protocol::Detail::ResponseVariant sub_type,
							  RequestId                                                request_id,
							  const Payload                                           &payload) {
					return ResponseFrame{
						Frames::Parser::calculate_declared_size(payload.size(), type),
						request_id,
						sub_type,
						payload,
						type};
				}

				template <ByteRange Payload>
				RequestFrame make_request(RequestSubTypes sub_type,
										  RequestId       id,
										  Resource        resource,
										  const Payload  &payload) {
					return RequestFrame{
						Frames::Parser::calculate_declared_size(payload.size(),
																BinaryTypes::Request),
						id,
						sub_type,
						payload,
						resource};
				}

				template <ByteRange Payload>
				std::vector<std::byte>
				make_response_raw_bytes(BinaryTypes                                   type,
										std::variant<ErrorSubTypes, ResponseSubTypes> sub_type,
										RequestId                                     request_id,
										const Payload                                &payload) {
					if (type == BinaryTypes::Response
						&& !std::holds_alternative<ResponseSubTypes>(sub_type)) {
						throw FrameInvariantError(std::format(
							"Sub type of a frame with type Response should be Response sub type"));
					} else if (type == BinaryTypes::Error
							   && !std::holds_alternative<ErrorSubTypes>(sub_type)) {
						throw FrameInvariantError(std::format(
							"Sub type of a frame with type Error should be Error sub type"));
					}


					std::vector<std::byte> frame;

					const Protocol::Aliases::FrameDeclaredSize declared_size =
						ProtocolLayout::SubType::SIZE + ProtocolLayout::Type::SIZE + payload.size()
						+ ProtocolLayout::Request::Id::SIZE;

					frame.reserve(declared_size + ProtocolLayout::DeclaredSize::SIZE);

					// declared size

					append_declared_size(frame, declared_size);

					// type

					append_type(frame, type);

					// sub type

					Protocol::Aliases::FrameSubType v_sub_type{};

					if (std::holds_alternative<ResponseSubTypes>(sub_type)) {
						v_sub_type = static_cast<Protocol::Aliases::FrameSubType>(
							std::get<ResponseSubTypes>(sub_type));
					} else if (std::holds_alternative<ErrorSubTypes>(sub_type)) {
						v_sub_type = static_cast<Protocol::Aliases::FrameSubType>(
							std::get<ErrorSubTypes>(sub_type));
					}


					Karkinolution::Binary::Utils::append_bytes(frame,
															   static_cast<std::byte>(v_sub_type));

					// request id


					Karkinolution::Binary::Utils::append_bytes(
						frame,
						Serializer::serialize<RequestId>(request_id));
					// payload
					frame.insert(frame.end(), payload.begin(), payload.end());
					return frame;
				}

				template <ByteRange Payload>
				std::vector<std::byte> make_request_raw_bytes(RequestSubTypes sub_type,
															  RequestId       id,
															  Resource        resource,
															  const Payload  &payload) {
					std::vector<std::byte>  frame;
					const FrameDeclaredSize declared_size = ProtocolLayout::SubType::SIZE
						+ ProtocolLayout::Type::SIZE + payload.size()
						+ ProtocolLayout::Request::Resource::SIZE
						+ ProtocolLayout::Request::Id::SIZE;


					frame.reserve(declared_size + ProtocolLayout::DeclaredSize::SIZE);

					// declared size
					append_declared_size(frame, declared_size);

					// type
					append_type(frame, BinaryTypes::Request);

					// sub type

					Karkinolution::Binary::Utils::append_bytes(
						frame,
						Serializer::serialize<Protocol::Aliases::FrameSubType>(
							static_cast<Protocol::Aliases::FrameSubType>(sub_type)));


					// id

					Karkinolution::Binary::Utils::append_bytes(
						frame,
						Serializer::serialize<RequestId>(id));

					// resource
					Karkinolution::Binary::Utils::append_bytes(
						frame,
						Serializer::serialize<Protocol::Aliases::FrameResource>(
							static_cast<Protocol::Aliases::FrameResource>(resource)));
					// payload
					frame.insert(frame.end(), payload.begin(), payload.end());
					return frame;
				}

			} // namespace Motor
		} // namespace Frames
	} // namespace Binary
} // namespace Karkinolution
