
#include "karkinolution/binary/core_aliases.hpp"

#include <karkinolution/binary/frames/models.hpp>
#include <karkinolution/binary/frames/parser/error/parser.hpp>
#include <karkinolution/binary/frames/parser/internal.hpp>
#include <karkinolution/binary/frames/parser/request/parser.hpp>
#include <karkinolution/binary/frames/parser/response/parser.hpp>

namespace Karkinolution {

	namespace Binary {
		namespace Frames {
			namespace Parser {
				enum class FrameExistence : uint8_t {
					YES,
					INCOMPLETE,
					NO
				};

				template <KShared::Binary::ByteRange T>
				FrameExistence has_frame(const T &bytes, std::size_t fixed_header_size) {
					if (bytes.size() == 0) {
						return FrameExistence::NO;
					}
					if (bytes.size() < ProtocolLayout::DeclaredSize::END) {
						return FrameExistence::INCOMPLETE;
					}

					const auto declared_size         = get_declared_size(bytes);
					const auto minimum_declared_size = fixed_header_size - ProtocolLayout::DeclaredSize::SIZE;
					if (declared_size < minimum_declared_size) {
						throw KShared::Binary::ByteError(
							std::format("Declared size {} is lower than the minimum frame size {}",
										declared_size,
										minimum_declared_size));
					}

					const auto frame_size = ProtocolLayout::DeclaredSize::SIZE + declared_size;
					return bytes.size() >= frame_size ? FrameExistence::YES : FrameExistence::INCOMPLETE;
				}

				template <KShared::Binary::ByteRange T> std::vector<std::byte> get_frame(const T &bytes) {
					const auto             frame_size = ProtocolLayout::DeclaredSize::SIZE + get_declared_size(bytes);
					std::vector<std::byte> frame;
					frame.reserve(frame_size);
					for (std::size_t i = 0; i < frame_size; ++i) {
						frame.push_back(bytes[i]);
					}
					return frame;
				}

				// The public dispatchers route by frame type and CodeVariant subtype.
				constexpr std::uint32_t calculate_declared_size(std::uint32_t                        payload_size,
																BinaryTypes                          type,
																const Protocol::Detail::CodeVariant &sub_type) {
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

				constexpr std::uint32_t calculate_payload_size(std::uint32_t                        declared_size,
															   BinaryTypes                          type,
															   const Protocol::Detail::CodeVariant &sub_type) {
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
				constexpr std::uint32_t calculate_payload_size(const T                             &bytes,
															   BinaryTypes                          type,
															   const Protocol::Detail::CodeVariant &sub_type) {
					return calculate_payload_size(get_declared_size(bytes), type, sub_type);
				}

				template <KShared::Binary::ByteRange T> std::vector<std::byte> get_payload(const T &bytes) {
					const auto type = get_type(bytes);
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
					return ProtocolLayout::DeclaredSize::SIZE + get_declared_size(bytes);
				}

				template <KShared::Binary::ByteRange T> Models::Request::Request parse_request_frame(const T &bytes) {
					const auto sub_type = std::get<RequestSubTypes>(get_sub_type(bytes));
					const auto resource = Request::get_resource(bytes);

					Models::Request::RequestExtra extra = std::monostate();

					switch (sub_type) {
						case Protocol::Request::Operation::SEE:
							extra = Models::Request::SeeExtra{.target = Request::See::get_target(bytes)};
							break;
						case Protocol::Request::Operation::WANT:
							extra = Models::Request::WantExtra{};
							break;
						case Protocol::Request::Operation::ERASE:
							extra = Models::Request::EraseExtra{};
							break;
						case Protocol::Request::Operation::CREATE:
							extra = Models::Request::CreateExtra{};
							break;
						case Protocol::Request::Operation::CYCLE:
							extra = Models::Request::CycleExtra{};
							break;
						default:
							KShared::Functional::unreachable_or_throw();
					}
					return Models::Request::Request{get_declared_size(bytes),
													Request::get_id(bytes),
													sub_type,
													get_payload(bytes),
													resource,
													std::move(extra)};
				}

				template <KShared::Binary::ByteRange T>
				Models::Response::Response parse_response_frame(const T &bytes) {
					return Models::Response::Response{get_declared_size(bytes),
													  Response::get_request_id(bytes),
													  std::get<ResponseSubTypes>(get_sub_type(bytes)),
													  Response::get_payload(bytes),
													  Response::get_cardinality(bytes)};
				}

				template <KShared::Binary::ByteRange T> Models::Error::Error parse_error_frame(const T &bytes) {
					return Models::Error::Error{get_declared_size(bytes),
												Error::get_request_id(bytes),
												std::get<ErrorSubTypes>(get_sub_type(bytes)),
												Error::get_payload(bytes)};
				}

				template <KShared::Binary::ByteRange T> Models::GenericFrame parse_frame(const T &bytes) {
					const auto type = get_type(bytes);
					Karkinolution::Binary::Protocol::Validator::validate_type(static_cast<std::uint8_t>(type));
					if (type == BinaryTypes::Request) {
						return Models::GenericFrame{parse_request_frame(bytes)};
					} else if (type == BinaryTypes::Response) {
						return Models::GenericFrame{parse_response_frame(bytes)};
					} else if (type == BinaryTypes::Error) {
						return Models::GenericFrame{parse_error_frame(bytes)};
					}
					KShared::Functional::unreachable_or_throw();
				}
			} // namespace Parser
		} // namespace Frames
	} // namespace Binary
} // namespace Karkinolution