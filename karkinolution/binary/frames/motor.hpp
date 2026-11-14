#pragma once


#include "karkinolution/binary/frames/errors.hpp"
#include "karkinolution/binary/frames/models.hpp"
#include "karkinolution/binary/frames/parser.hpp"
#include "karkinolution/binary/frames/processor.hpp"
#include "karkinolution/binary/protocol/aliases.hpp"
#include "karkinolution/binary/protocol/contract.hpp"
#include "shared/binary/byte_range.hpp"
#include "shared/binary/byte_utils.hpp"

#include <array>
#include <karkinolution/binary/core_aliases.hpp>
#include <karkinolution/binary/frames/aliases.hpp>
#include <karkinolution/binary/protocol/layouts.hpp>
#include <karkinolution/binary/protocol/utils.hpp>
#include <karkinolution/networking/scope/layout.hpp>
#include <shared/binary/deserializer.hpp>
#include <shared/binary/serializer.hpp>
#include <vector>

namespace Karkinolution {
	namespace Binary {
		namespace Frames {
			namespace Motor {
				template <KShared::Binary::ByteRange Payload>
				std::vector<std::byte> copy_payload(const Payload &payload) {
					std::vector<std::byte> copied_payload;
					copied_payload.reserve(payload.size());
					for (std::size_t index = 0; index < payload.size(); ++index) {
						copied_payload.push_back(payload[index]);
					}
					return copied_payload;
				}

				void constexpr append_declared_size(std::vector<std::byte>              &frame,
													Protocol::Aliases::FrameDeclaredSize declared_size) {
					KShared::Binary::Utils::append_bytes(
						frame,
						KShared::Binary::Serializer::serialize<Protocol::Aliases::FrameDeclaredSize>(declared_size));
				}

				void constexpr append_type(std::vector<std::byte> &frame, BinaryTypes type) {
					KShared::Binary::Utils::append_bytes(
						frame,
						KShared::Binary::Serializer::serialize(static_cast<Protocol::Aliases::FrameType>(type)));
				}

				void constexpr append_sub_type(std::vector<std::byte> &frame, Protocol::Detail::CodeVariant sub_type) {
					KShared::Binary::Utils::append_bytes(
						frame,
						KShared::Binary::Serializer::serialize(Utils::cast_numeric(sub_type)));
				}

				ResponseFrame make_none_response(RequestId request_id);

				template <KShared::Binary::ByteRange Payload>
				ResponseFrame make_response(Protocol::Response::Code sub_type,
											RequestId                request_id,
											const Payload           &payload,
											ResponseCardinality      cardinality) {
					return ResponseFrame{
						Frames::Parser::calculate_declared_size(payload.size(),
																BinaryTypes::Response,
																Protocol::Detail::CodeVariant{sub_type}),
						request_id,
						sub_type,
						copy_payload(payload),
						cardinality};
				}

				template <KShared::Binary::ByteRange T>
				ErrorFrame make_error(Karkinolution::Binary::Protocol::Error::Code sub_type,
									  RequestId                                    request_id,
									  const T                                     &payload) {
					return ErrorFrame{Frames::Parser::calculate_declared_size(payload.size(),
																			  BinaryTypes::Error,
																			  Protocol::Detail::CodeVariant{sub_type}),
									  request_id,
									  sub_type,
									  copy_payload(payload)};
				}

				template <KShared::Binary::ByteRange Payload>
				RequestFrame make_request(RequestSubTypes                      sub_type,
										  RequestId                            id,
										  Resource                             resource,
										  const Payload                       &payload,
										  const Models::Request::RequestExtra &extra) {

					return RequestFrame{
						Frames::Parser::calculate_declared_size(payload.size(),
																BinaryTypes::Request,
																Protocol::Detail::CodeVariant{sub_type}),
						id,
						sub_type,
						copy_payload(payload),
						resource,
						extra};
				}

				template <KShared::Binary::ByteRange T>

				std::vector<std::byte> make_raw_response(ResponseCardinality cardinality,
														 ResponseSubTypes    sub_type,
														 RequestId           request_id,
														 const T            &payload) {
					std::vector<std::byte> frame;

					const auto declared_size =
						Parser::calculate_declared_size(payload.size(), BinaryTypes::Response, sub_type);

					// validation
					Models::Response::Response{declared_size, request_id, sub_type, copy_payload(payload), cardinality};

					frame.reserve(declared_size + ProtocolLayout::DeclaredSize::SIZE);

					// declared size

					append_declared_size(frame, declared_size);

					// type

					append_type(frame, BinaryTypes::Response);

					// sub type


					append_sub_type(frame, sub_type);

					// request id


					KShared::Binary::Utils::append_bytes(frame,
														 KShared::Binary::Serializer::serialize<RequestId>(request_id));


					KShared::Binary::Utils::append_bytes(
						frame,
						KShared::Binary::Serializer::serialize<Protocol::Aliases::Response::Cardinality>(
							static_cast<Protocol::Aliases::Response::Cardinality>(cardinality)));
					// payload
					frame.insert(frame.end(), payload.begin(), payload.end());
					return frame;
				}

				template <KShared::Binary::ByteRange T>
				std::vector<std::byte> make_raw_error(ErrorSubTypes sub_type, RequestId request_id, const T &payload) {
					const auto declared_size =
						Frames::Parser::calculate_declared_size(payload.size(), BinaryTypes::Error, sub_type);
					std::vector<std::byte> frame;
					frame.reserve(declared_size + ProtocolLayout::DeclaredSize::SIZE);

					// declared size

					append_declared_size(frame, declared_size);

					// type

					append_type(frame, BinaryTypes::Error);

					// sub type


					append_sub_type(frame, sub_type);

					// request id


					KShared::Binary::Utils::append_bytes(frame,
														 KShared::Binary::Serializer::serialize<RequestId>(request_id));

					// payload
					frame.insert(frame.end(), payload.begin(), payload.end());
					return frame;
				}

				template <KShared::Binary::ByteRange T>
				std::vector<std::byte>
				make_response_raw_bytes(BinaryTypes                                   type,
										std::variant<ErrorSubTypes, ResponseSubTypes> sub_type,
										RequestId                                     request_id,
										const T                                      &payload,
										ResponseCardinality cardinality = ResponseCardinality::ONE) {
					if (type == BinaryTypes::Response) {
						if (!std::holds_alternative<ResponseSubTypes>(sub_type)) {
							throw FrameInvariantError("Response frames require a response subtype");
						}
						return make_raw_response(cardinality,
												 std::get<ResponseSubTypes>(sub_type),
												 request_id,
												 payload);
					}
					if (type == BinaryTypes::Error) {
						if (!std::holds_alternative<ErrorSubTypes>(sub_type)) {
							throw FrameInvariantError("Error frames require an error subtype");
						}
						return make_raw_error(std::get<ErrorSubTypes>(sub_type), request_id, payload);
					}
					throw FrameInvariantError("Response raw bytes require a response or error frame type");
				}

				template <KShared::Binary::ByteRange Payload>
				std::vector<std::byte> make_request_raw_bytes(RequestSubTypes                      sub_type,
															  RequestId                            id,
															  Resource                             resource,
															  const Payload                       &payload,
															  const Models::Request::RequestExtra &extra) {

					const FrameDeclaredSize declared_size =
						Parser::calculate_declared_size(payload.size(),
														BinaryTypes::Request,
														Protocol::Detail::CodeVariant{sub_type});
					const Models::Request::Request request{declared_size,
														   id,
														   sub_type,
														   copy_payload(payload),
														   resource,
														   extra};

					std::vector<std::byte> frame;
					frame.reserve(request.get_declared_size() + ProtocolLayout::DeclaredSize::SIZE);

					// declared size
					append_declared_size(frame, request.get_declared_size());

					// type
					append_type(frame, BinaryTypes::Request);

					// sub type

					append_sub_type(frame, sub_type);

					// id

					KShared::Binary::Utils::append_bytes(
						frame,
						KShared::Binary::Serializer::serialize<RequestId>(request.get_id()));

					// resource
					KShared::Binary::Utils::append_bytes(
						frame,
						KShared::Binary::Serializer::serialize<Protocol::Aliases::Request::FrameResource>(
							static_cast<Protocol::Aliases::Request::FrameResource>(request.get_resource())));

					if (request.get_sub_type() == RequestSubTypes::SEE) {
						const auto target = std::get<Models::Request::SeeExtra>(request.get_extra()).target;
						KShared::Binary::Utils::append_bytes(
							frame,
							KShared::Binary::Serializer::serialize<Protocol::Aliases::Request::FrameSeeTarget>(
								static_cast<Protocol::Aliases::Request::FrameSeeTarget>(target)));
					}

					// payload
					const auto &validated_payload = request.get_payload();
					frame.insert(frame.end(), validated_payload.begin(), validated_payload.end());
					return frame;
				}

			} // namespace Motor
		} // namespace Frames
	} // namespace Binary
} // namespace Karkinolution
