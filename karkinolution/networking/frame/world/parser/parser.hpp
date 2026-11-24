#pragma once
#include "karkinolution/networking/frame/world/core_aliases.hpp"
#include "shared/binary/byte_range.hpp"
#include "shared/functional/unreachable_or_throw.hpp"

#include <karkinolution/networking/frame/world/models.hpp>
#include <karkinolution/networking/frame/world/parser/error/parser.hpp>
#include <karkinolution/networking/frame/world/parser/internal.hpp>
#include <karkinolution/networking/frame/world/parser/request/parser.hpp>
#include <karkinolution/networking/frame/world/parser/response/parser.hpp>

namespace Karkinolution {

	namespace Networking {
		namespace Frames::World {
			namespace Parser {


				template <KShared::Binary::ByteRange T> std::vector<std::byte> get_frame(const T &bytes) {
					const auto             frame_size = Layout::DeclaredSize::SIZE + Internal::get_declared_size(bytes);
					std::vector<std::byte> frame;
					frame.reserve(frame_size);
					for (std::size_t i = 0; i < frame_size; ++i) {
						frame.push_back(bytes[i]);
					}
					return frame;
				}

				// The public dispatchers route by frame type and CodeVariant subtype.
				constexpr std::uint32_t calculate_declared_size(std::uint32_t                 payload_size,
																BinaryTypes                   type,
																const Detail::SubTypeVariant &sub_type) {
					if (type == BinaryTypes::Request) {
						if (const auto* request_sub_type = std::get_if<RequestSubTypes>(&sub_type)) {
							return Request::calculate_declared_size(payload_size, *request_sub_type);
						}
					} else if (type == BinaryTypes::Response) {
						if (std::holds_alternative<ResponseSubTypes>(sub_type)) {
							return Response::calculate_declared_size(payload_size);
						}
					} else if (type == BinaryTypes::Error) {
						if (std::holds_alternative<ErrorSubTypes>(sub_type)) {
							return Error::calculate_declared_size(payload_size);
						}
					}
					KShared::Functional::unreachable_or_throw("Frame type and sub type do not match");
				}

				constexpr std::uint32_t calculate_payload_size(std::uint32_t                 declared_size,
															   BinaryTypes                   type,
															   const Detail::SubTypeVariant &sub_type) {
					if (type == BinaryTypes::Request) {
						if (const auto* request_sub_type = std::get_if<RequestSubTypes>(&sub_type)) {
							return Request::calculate_payload_size(declared_size, *request_sub_type);
						}
					} else if (type == BinaryTypes::Response) {
						if (std::holds_alternative<ResponseSubTypes>(sub_type)) {
							return Response::calculate_payload_size(declared_size);
						}
					} else if (type == BinaryTypes::Error) {
						if (std::holds_alternative<ErrorSubTypes>(sub_type)) {
							return Error::calculate_payload_size(declared_size);
						}
					}
					throw KShared::Binary::ByteError("Frame type and sub type do not match");
				}

				template <KShared::Binary::ByteRange T>
				constexpr std::uint32_t
				calculate_payload_size(const T &bytes, BinaryTypes type, const Detail::SubTypeVariant &sub_type) {
					return calculate_payload_size(get_declared_size(bytes), type, sub_type);
				}

				template <KShared::Binary::ByteRange T> std::vector<std::byte> get_payload(const T &bytes) {
					const auto type = Internal::get_type(bytes);
					if (type == BinaryTypes::Request) {
						return Request::get_payload(bytes);
					} else if (type == BinaryTypes::Response) {
						return Response::get_payload(bytes);
					} else if (type == BinaryTypes::Error) {
						return Error::get_payload(bytes);
					}
					KShared::Functional::unreachable_or_throw();
				}

				template <KShared::Binary::ByteRange T> std::uint32_t get_frame_size(const T &bytes) {
					return Layout::DeclaredSize::SIZE + Internal::get_declared_size(bytes);
				}

				template <KShared::Binary::ByteRange T> Models::Request::Request parse_request_frame(const T &bytes) {
					const auto sub_type = std::get<RequestSubTypes>(Internal::get_sub_type(bytes));
					const auto resource = Request::get_resource(bytes);

					Models::Request::RequestExtra extra = std::monostate();

					switch (sub_type) {
						case RequestSubTypes::SEE:
							extra = Models::Request::SeeExtra{.target = Request::See::get_target(bytes)};
							break;
						case RequestSubTypes::WANT:
							extra = Models::Request::WantExtra{};
							break;
						case RequestSubTypes::ERASE:
							extra = Models::Request::EraseExtra{};
							break;
						case RequestSubTypes::CREATE:
							extra = Models::Request::CreateExtra{};
							break;
						case RequestSubTypes::CYCLE:
							extra = Models::Request::CycleExtra{};
							break;
						default:
							KShared::Functional::unreachable_or_throw();
					}
					return Models::Request::Request{Internal::get_declared_size(bytes),
													Request::get_id(bytes),
													sub_type,
													get_payload(bytes),
													resource,
													std::move(extra)};
				}

				template <KShared::Binary::ByteRange T>
				Models::Response::Response parse_response_frame(const T &bytes) {
					return Models::Response::Response{Internal::get_declared_size(bytes),
													  Response::get_request_id(bytes),
													  std::get<ResponseSubTypes>(Internal::get_sub_type(bytes)),
													  Response::get_payload(bytes),
													  Response::get_cardinality(bytes)};
				}

				template <KShared::Binary::ByteRange T> Models::Error::Error parse_error_frame(const T &bytes) {
					return Models::Error::Error{Internal::get_declared_size(bytes),
												Error::get_request_id(bytes),
												std::get<ErrorSubTypes>(Internal::get_sub_type(bytes)),
												Error::get_payload(bytes)};
				}

				template <KShared::Binary::ByteRange T> Models::GenericFrame parse_frame(const T &bytes) {
					const auto type = get_type(bytes);
					Karkinolution::Networking::Frames::World::Validator::validate_type(static_cast<std::uint8_t>(type));
					if (type == BinaryTypes::Request) {
						return Models::GenericFrame{parse_request_frame(bytes)};
					} else if (type == BinaryTypes::Response) {
						return Models::GenericFrame{parse_response_frame(bytes)};
					} else if (type == BinaryTypes::Error) {
						return Models::GenericFrame{parse_error_frame(bytes)};
					}
					KShared::Functional::unreachable_or_throw();
				}

				template <KShared::Binary::ByteRange T> Internal::FrameExistence has_frame(const T &bytes) {
					if (Internal::has_header(bytes) != Internal::FrameExistence::YES) {
						return Internal::has_header(bytes);
					}
					const auto type = Internal::get_type(bytes);

					if (type == BinaryTypes::Request) {
						return Request::has_frame(bytes);
					} else if (type == BinaryTypes::Response) {
						return Response::has_frame(bytes);
					} else if (type == BinaryTypes::Error) {
						return Error::has_frame(bytes);
					}
					KShared::Functional::unreachable_or_throw();
				}
			} // namespace Parser
		} // namespace Frames::World
	} // namespace Networking
} // namespace Karkinolution