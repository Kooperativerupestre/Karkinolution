#pragma once


#include "karkinolution/networking/frame/errors.hpp"
#include "karkinolution/networking/frame/world/contract.hpp"
#include "karkinolution/networking/frame/world/core_aliases.hpp"
#include "karkinolution/networking/frame/world/models.hpp"
#include "karkinolution/networking/frame/world/processor.hpp"
#include "shared/binary/byte_range.hpp"
#include "shared/binary/byte_utils.hpp"

#include <array>
#include <karkinolution/networking/frame/world/aliases.hpp>
#include <karkinolution/networking/frame/world/core_aliases.hpp>
#include <karkinolution/networking/frame/world/layout.hpp>
#include <karkinolution/networking/frame/world/parser/parser.hpp>
#include <karkinolution/networking/frame/world/utils.hpp>
#include <karkinolution/networking/scope/layout.hpp>
#include <shared/binary/deserializer.hpp>
#include <shared/binary/serializer.hpp>
#include <vector>

namespace Karkinolution {
	namespace Networking {
		namespace Frames::World {
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

				void constexpr append_declared_size(std::vector<std::byte> &frame, DeclaredSize declared_size) {
					KShared::Binary::Utils::append_bytes(
						frame,
						KShared::Binary::Serializer::serialize<DeclaredSize>(declared_size));
				}

				void constexpr append_type(std::vector<std::byte> &frame, BinaryTypes type) {
					KShared::Binary::Utils::append_bytes(
						frame,
						KShared::Binary::Serializer::serialize(static_cast<Detail::UnderlyingType<Type>>(type)));
				}

				void constexpr append_sub_type(std::vector<std::byte> &frame, Detail::SubTypeVariant sub_type) {
					KShared::Binary::Utils::append_bytes(
						frame,
						KShared::Binary::Serializer::serialize(Utils::cast_numeric(sub_type)));
				}

				ResponseFrame make_none_response(RequestId request_id);
				ResponseFrame make_boolean_response(RequestId request_id, bool value);

				template <KShared::Binary::ByteRange Payload>
				ResponseFrame make_response(Response::SubType   sub_type,
											RequestId           request_id,
											const Payload      &payload,
											ResponseCardinality cardinality) {
					return ResponseFrame{Parser::calculate_declared_size(payload.size(),
																		 BinaryTypes::Response,
																		 Detail::SubTypeVariant{sub_type}),
										 request_id,
										 sub_type,
										 copy_payload(payload),
										 cardinality};
				}

				template <KShared::Binary::ByteRange T>
				ErrorFrame make_error(Karkinolution::Networking::Frames::World::Error::SubType sub_type,
									  RequestId                                                request_id,
									  const T                                                 &payload) {
					return ErrorFrame{Parser::calculate_declared_size(payload.size(),
																	  BinaryTypes::Error,
																	  Detail::SubTypeVariant{sub_type}),
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

					return RequestFrame{Parser::calculate_declared_size(payload.size(),
																		BinaryTypes::Request,
																		Detail::SubTypeVariant{sub_type}),
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

					frame.reserve(declared_size + Layout::DeclaredSize::SIZE);

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
						KShared::Binary::Serializer::serialize<Detail::UnderlyingType<Response::Cardinality>>(
							static_cast<Detail::UnderlyingType<Response::Cardinality>>(cardinality)));
					// payload
					frame.insert(frame.end(), payload.begin(), payload.end());
					return frame;
				}

				template <KShared::Binary::ByteRange T>
				std::vector<std::byte> make_raw_error(ErrorSubTypes sub_type, RequestId request_id, const T &payload) {
					const auto declared_size =
						Parser::calculate_declared_size(payload.size(), BinaryTypes::Error, sub_type);
					std::vector<std::byte> frame;
					frame.reserve(declared_size + Layout::DeclaredSize::SIZE);

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
														Detail::SubTypeVariant{sub_type});
					const Models::Request::Request request{declared_size,
														   id,
														   sub_type,
														   copy_payload(payload),
														   resource,
														   extra};

					std::vector<std::byte> frame;
					frame.reserve(request.get_declared_size() + Layout::DeclaredSize::SIZE);

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
						KShared::Binary::Serializer::serialize<Detail::UnderlyingType<Request::Resource>>(
							static_cast<Detail::UnderlyingType<Request::Resource>>(request.get_resource())));

					if (request.get_sub_type() == RequestSubTypes::SEE) {
						const auto target = std::get<Models::Request::SeeExtra>(request.get_extra()).target;
						KShared::Binary::Utils::append_bytes(
							frame,
							KShared::Binary::Serializer::serialize<Detail::UnderlyingType<Request::SeeTarget>>(
								static_cast<Detail::UnderlyingType<Request::SeeTarget>>(target)));
					}

					// payload
					const auto &validated_payload = request.get_payload();
					frame.insert(frame.end(), validated_payload.begin(), validated_payload.end());
					return frame;
				}

			} // namespace Motor
		} // namespace Frames::World
	} // namespace Networking
} // namespace Karkinolution
