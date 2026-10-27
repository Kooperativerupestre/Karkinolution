#pragma once
#include "karkinolution/binary/core_aliases.hpp"
#include "karkinolution/binary/frames/invariants.hpp"
#include "karkinolution/binary/protocol/aliases.hpp"
#include "karkinolution/binary/protocol/contract.hpp"
#include "karkinolution/core/error.hpp"
#include "karkinolution/functional/unreachable_or_throw.hpp"

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

using BinaryTypes = Karkinolution::Binary::Protocol::Type;

namespace Karkinolution {
	namespace Binary {

		namespace Frames {

			namespace Parser {
				enum FrameExistence : uint8_t {
					YES,
					INCOMPLETE,
					NO
				};

				namespace Internal {
					constexpr std::uint32_t calculate_declared_size(
						std::uint32_t payload_size,
						std::size_t   fixed_header_size) {
						return fixed_header_size - ProtocolLayout::DeclaredSize::SIZE
							+ payload_size;
					}

					constexpr std::uint32_t calculate_payload_size(
						std::uint32_t declared_size,
						std::size_t   fixed_header_size) {
						const auto minimum_declared_size =
							fixed_header_size - ProtocolLayout::DeclaredSize::SIZE;

						if (declared_size < minimum_declared_size) {
							throw ByteError(std::format(
								"Cannot get payload bytes because size {} < minimum size {}",
								declared_size,
								minimum_declared_size));
						}
						return declared_size - minimum_declared_size;
					}

					template <ByteRange T>
					constexpr std::uint32_t calculate_payload_size(
						const T   &bytes,
						std::size_t fixed_header_size) {
						const auto declared_size =
							Deserializer::read_uint32_t(bytes, ProtocolLayout::DeclaredSize::OFFSET);
						return calculate_payload_size(declared_size, fixed_header_size);
					}

					template <ByteRange T>
					FrameExistence has_frame(const T &bytes, std::size_t fixed_header_size) {
						if (bytes.size() == 0) {
							return FrameExistence::NO;
						}
						if (bytes.size() < ProtocolLayout::DeclaredSize::END) {
							return FrameExistence::INCOMPLETE;
						}

						const auto declared_size =
							Deserializer::read_uint32_t(bytes, ProtocolLayout::DeclaredSize::OFFSET);
						const auto minimum_declared_size =
							fixed_header_size - ProtocolLayout::DeclaredSize::SIZE;
						if (declared_size < minimum_declared_size) {
							throw ByteError(std::format(
								"Declared size {} is lower than the minimum frame size {}",
								declared_size,
								minimum_declared_size));
						}

						const auto frame_size =
							ProtocolLayout::DeclaredSize::SIZE + declared_size;
						return bytes.size() >= frame_size ? FrameExistence::YES
														  : FrameExistence::INCOMPLETE;
					}

					template <ByteRange T>
					std::vector<std::byte> get_frame(const T &bytes, std::size_t fixed_header_size) {
						if (has_frame(bytes, fixed_header_size) != FrameExistence::YES) {
							throw ByteError("Cannot extract a frame from an incomplete buffer");
						}

						const auto frame_size = ProtocolLayout::DeclaredSize::SIZE
							+ Deserializer::read_uint32_t(
								bytes,
								ProtocolLayout::DeclaredSize::OFFSET);
						std::vector<std::byte> frame;
						frame.reserve(frame_size);
						for (std::size_t i = 0; i < frame_size; ++i) {
							frame.push_back(bytes[i]);
						}
						return frame;
					}
				} // namespace Internal

				template <ByteRange T> std::uint32_t get_declared_size(const T &bytes) {
					return Deserializer::read_uint32_t(bytes, 0);
				}

				template <ByteRange T> BinaryTypes get_type(const T &bytes) {
					const auto raw =
						Deserializer::read_uint8_t(bytes, ProtocolLayout::DeclaredSize::SIZE);

					Protocol::Validator::validate_type(raw);
					return static_cast<BinaryTypes>(raw);
				}

				template <ByteRange T>
				Karkinolution::Binary::Protocol::Detail::CodeVariant get_sub_type(const T &bytes) {
					Karkinolution::Binary::Protocol::Detail::CodeVariant sub_type;
					const auto                                           type = get_type(bytes);
					const auto                                           read =
						Deserializer::read_uint8_t(bytes, ProtocolLayout::SubType::OFFSET);

					Karkinolution::Binary::Protocol::Validator::validate_sub_type(type, read);

					if (type == BinaryTypes::Error) {
						return ErrorSubTypes{read};
					} else if (type == BinaryTypes::Request) {
						return RequestSubTypes{read};
					} else if (type == BinaryTypes::Response) {
						return ResponseSubTypes{read};
					}
					Karkinolution::Functional::unreachable_or_throw();
				}

				template <ByteRange T> bool has_size(const T &byte) {
					return byte.size() >= ProtocolLayout::DeclaredSize::END;
				}

				template <ByteRange T> bool has_type(const T &byte) {
					return byte.size() >= ProtocolLayout::Type::END;
				}

				template <ByteRange T> bool has_sub_type(const T &byte) {
					return byte.size() >= ProtocolLayout::SubType::END;
				}

				namespace Request {
					template <ByteRange T> RequestId get_id(const T &bytes) {
						return Deserializer::deserialize<RequestId>(
							bytes,
							ProtocolLayout::Request::Id::OFFSET);
					}

					template <ByteRange T> bool has_id(const T &bytes) {
						return bytes.size() >= ProtocolLayout::Request::Id::END;
					}

					template <ByteRange T> Resource get_resource(const T &bytes) {
						auto read = Deserializer::deserialize<Protocol::Aliases::FrameSubType>(
							bytes,
							ProtocolLayout::Request::Resource::OFFSET);

						Karkinolution::Binary::Protocol::Validator::Request::validate_resource(
							read);
						return Resource(read);
					}

					template <ByteRange T> bool has_resource(const T &bytes) {
						return bytes.size() >= ProtocolLayout::Request::Resource::END;
					}

					namespace See {
						template <ByteRange T>
						Protocol::Request::SeeTarget get_target(const T &bytes) {
							return static_cast<Protocol::Request::SeeTarget>(
								Deserializer::deserialize<
									Protocol::Aliases::Request::FrameSeeTarget>(
									bytes,
									ProtocolLayout::Request::See::Target::OFFSET));
						}

						template <ByteRange T> bool has_target(const T &bytes) {
							return bytes.size() >= ProtocolLayout::Request::See::Target::END;
						}

						template <ByteRange T> std::vector<std::byte> get_payload(const T &bytes) {
							std::vector<std::byte> payload;
							payload.insert(payload.end(),
										   bytes.begin()
											   + ProtocolLayout::Request::See::Payload::OFFSET,
										   bytes.begin()
											   + ProtocolLayout::Request::See::Payload::OFFSET
											   + Internal::calculate_payload_size(
												   bytes,
												   ProtocolLayout::Request::See::FIXED_HEADER_SIZE));
							return payload;
						}

						constexpr std::uint32_t
						calculate_declared_size(std::uint32_t payload_size) {
							return Internal::calculate_declared_size(
								payload_size,
								ProtocolLayout::Request::See::FIXED_HEADER_SIZE);
						}

						constexpr std::uint32_t calculate_payload_size(std::uint32_t declared_size) {
							return Internal::calculate_payload_size(
								declared_size,
								ProtocolLayout::Request::See::FIXED_HEADER_SIZE);
						}

						template <ByteRange T>
						constexpr std::uint32_t calculate_payload_size(const T &bytes) {
							return Internal::calculate_payload_size(
								bytes,
								ProtocolLayout::Request::See::FIXED_HEADER_SIZE);
						}

						template <ByteRange T> bool has_payload(const T &bytes) {
							const auto payload_size = calculate_payload_size(bytes);
							return bytes.size()
								>= ProtocolLayout::Request::See::Payload::OFFSET + payload_size;
						}

						template <ByteRange T> FrameExistence has_frame(const T &bytes) {
							return Internal::has_frame(
								bytes,
								ProtocolLayout::Request::See::FIXED_HEADER_SIZE);
						}

						template <ByteRange T> std::vector<std::byte> get_frame(const T &bytes) {
							return Internal::get_frame(
								bytes,
								ProtocolLayout::Request::See::FIXED_HEADER_SIZE);
						}
					} // namespace See

					namespace Create {
						template <ByteRange T> std::vector<std::byte> get_payload(const T &bytes) {
							std::vector<std::byte> payload;
							payload.insert(
								payload.end(),
								bytes.begin() + ProtocolLayout::Request::Create::Payload::OFFSET,
								bytes.begin() + ProtocolLayout::Request::Create::Payload::OFFSET
									+ Internal::calculate_payload_size(
										bytes,
										ProtocolLayout::Request::Create::FIXED_HEADER_SIZE));
							return payload;
						}

						constexpr std::uint32_t
						calculate_declared_size(std::uint32_t payload_size) {
							return Internal::calculate_declared_size(
								payload_size,
								ProtocolLayout::Request::Create::FIXED_HEADER_SIZE);
						}

						constexpr std::uint32_t calculate_payload_size(std::uint32_t declared_size) {
							return Internal::calculate_payload_size(
								declared_size,
								ProtocolLayout::Request::Create::FIXED_HEADER_SIZE);
						}

						template <ByteRange T>
						constexpr std::uint32_t calculate_payload_size(const T &bytes) {
							return Internal::calculate_payload_size(
								bytes,
								ProtocolLayout::Request::Create::FIXED_HEADER_SIZE);
						}

						template <ByteRange T> bool has_payload(const T &bytes) {
							const auto payload_size = calculate_payload_size(bytes);
							return bytes.size()
								>= ProtocolLayout::Request::Create::Payload::OFFSET + payload_size;
						}

						template <ByteRange T> FrameExistence has_frame(const T &bytes) {
							return Internal::has_frame(
								bytes,
								ProtocolLayout::Request::Create::FIXED_HEADER_SIZE);
						}

						template <ByteRange T> std::vector<std::byte> get_frame(const T &bytes) {
							return Internal::get_frame(
								bytes,
								ProtocolLayout::Request::Create::FIXED_HEADER_SIZE);
						}
					} // namespace Create

					namespace Cycle {
						template <ByteRange T> std::vector<std::byte> get_payload(const T &bytes) {
							std::vector<std::byte> payload;
							payload.insert(
								payload.end(),
								bytes.begin() + ProtocolLayout::Request::Cycle::Payload::OFFSET,
								bytes.begin() + ProtocolLayout::Request::Cycle::Payload::OFFSET
									+ Internal::calculate_payload_size(
										bytes,
										ProtocolLayout::Request::Cycle::FIXED_HEADER_SIZE));
							return payload;
						}

						constexpr std::uint32_t
						calculate_declared_size(std::uint32_t payload_size) {
							return Internal::calculate_declared_size(
								payload_size,
								ProtocolLayout::Request::Cycle::FIXED_HEADER_SIZE);
						}

						constexpr std::uint32_t calculate_payload_size(std::uint32_t declared_size) {
							return Internal::calculate_payload_size(
								declared_size,
								ProtocolLayout::Request::Cycle::FIXED_HEADER_SIZE);
						}

						template <ByteRange T>
						constexpr std::uint32_t calculate_payload_size(const T &bytes) {
							return Internal::calculate_payload_size(
								bytes,
								ProtocolLayout::Request::Cycle::FIXED_HEADER_SIZE);
						}

						template <ByteRange T> bool has_payload(const T &bytes) {
							const auto payload_size = calculate_payload_size(bytes);
							return bytes.size()
								>= ProtocolLayout::Request::Cycle::Payload::OFFSET + payload_size;
						}

						template <ByteRange T> FrameExistence has_frame(const T &bytes) {
							return Internal::has_frame(
								bytes,
								ProtocolLayout::Request::Cycle::FIXED_HEADER_SIZE);
						}

						template <ByteRange T> std::vector<std::byte> get_frame(const T &bytes) {
							return Internal::get_frame(
								bytes,
								ProtocolLayout::Request::Cycle::FIXED_HEADER_SIZE);
						}
					} // namespace Cycle

					namespace Erase {
						template <ByteRange T> std::vector<std::byte> get_payload(const T &bytes) {
							std::vector<std::byte> payload;
							payload.insert(
								payload.end(),
								bytes.begin() + ProtocolLayout::Request::Erase::Payload::OFFSET,
								bytes.begin() + ProtocolLayout::Request::Erase::Payload::OFFSET
									+ Internal::calculate_payload_size(
										bytes,
										ProtocolLayout::Request::Erase::FIXED_HEADER_SIZE));
							return payload;
						}

						constexpr std::uint32_t
						calculate_declared_size(std::uint32_t payload_size) {
							return Internal::calculate_declared_size(
								payload_size,
								ProtocolLayout::Request::Erase::FIXED_HEADER_SIZE);
						}

						constexpr std::uint32_t calculate_payload_size(std::uint32_t declared_size) {
							return Internal::calculate_payload_size(
								declared_size,
								ProtocolLayout::Request::Erase::FIXED_HEADER_SIZE);
						}

						template <ByteRange T>
						constexpr std::uint32_t calculate_payload_size(const T &bytes) {
							return Internal::calculate_payload_size(
								bytes,
								ProtocolLayout::Request::Erase::FIXED_HEADER_SIZE);
						}

						template <ByteRange T> bool has_payload(const T &bytes) {
							const auto payload_size = calculate_payload_size(bytes);
							return bytes.size()
								>= ProtocolLayout::Request::Erase::Payload::OFFSET + payload_size;
						}

						template <ByteRange T> FrameExistence has_frame(const T &bytes) {
							return Internal::has_frame(
								bytes,
								ProtocolLayout::Request::Erase::FIXED_HEADER_SIZE);
						}

						template <ByteRange T> std::vector<std::byte> get_frame(const T &bytes) {
							return Internal::get_frame(
								bytes,
								ProtocolLayout::Request::Erase::FIXED_HEADER_SIZE);
						}
					} // namespace Erase

					namespace Want {
						template <ByteRange T> std::vector<std::byte> get_payload(const T &bytes) {
							std::vector<std::byte> payload;
							payload.insert(
								payload.end(),
								bytes.begin() + ProtocolLayout::Request::Want::Payload::OFFSET,
								bytes.begin() + ProtocolLayout::Request::Want::Payload::OFFSET
									+ Internal::calculate_payload_size(
										bytes,
										ProtocolLayout::Request::Want::FIXED_HEADER_SIZE));
							return payload;
						}

						constexpr std::uint32_t
						calculate_declared_size(std::uint32_t payload_size) {
							return Internal::calculate_declared_size(
								payload_size,
								ProtocolLayout::Request::Want::FIXED_HEADER_SIZE);
						}

						constexpr std::uint32_t calculate_payload_size(std::uint32_t declared_size) {
							return Internal::calculate_payload_size(
								declared_size,
								ProtocolLayout::Request::Want::FIXED_HEADER_SIZE);
						}

						template <ByteRange T>
						constexpr std::uint32_t calculate_payload_size(const T &bytes) {
							return Internal::calculate_payload_size(
								bytes,
								ProtocolLayout::Request::Want::FIXED_HEADER_SIZE);
						}

						template <ByteRange T> bool has_payload(const T &bytes) {
							const auto payload_size = calculate_payload_size(bytes);
							return bytes.size()
								>= ProtocolLayout::Request::Want::Payload::OFFSET + payload_size;
						}

						template <ByteRange T> FrameExistence has_frame(const T &bytes) {
							return Internal::has_frame(
								bytes,
								ProtocolLayout::Request::Want::FIXED_HEADER_SIZE);
						}

						template <ByteRange T> std::vector<std::byte> get_frame(const T &bytes) {
							return Internal::get_frame(
								bytes,
								ProtocolLayout::Request::Want::FIXED_HEADER_SIZE);
						}
					} // namespace Want

					inline std::uint32_t calculate_declared_size(
						std::uint32_t payload_size,
						RequestSubTypes sub_type) {
						switch (sub_type) {
						case RequestSubTypes::SEE:
							return See::calculate_declared_size(payload_size);
						case RequestSubTypes::CREATE:
							return Create::calculate_declared_size(payload_size);
						case RequestSubTypes::CYCLE:
							return Cycle::calculate_declared_size(payload_size);
						case RequestSubTypes::ERASE:
							return Erase::calculate_declared_size(payload_size);
						case RequestSubTypes::WANT:
							return Want::calculate_declared_size(payload_size);
						}
						Karkinolution::Functional::unreachable_or_throw();
					}

					inline std::uint32_t calculate_payload_size(
						std::uint32_t declared_size,
						RequestSubTypes  sub_type) {
						switch (sub_type) {
						case RequestSubTypes::SEE:
							return See::calculate_payload_size(declared_size);
						case RequestSubTypes::CREATE:
							return Create::calculate_payload_size(declared_size);
						case RequestSubTypes::CYCLE:
							return Cycle::calculate_payload_size(declared_size);
						case RequestSubTypes::ERASE:
							return Erase::calculate_payload_size(declared_size);
						case RequestSubTypes::WANT:
							return Want::calculate_payload_size(declared_size);
						}
						Karkinolution::Functional::unreachable_or_throw();
					}

					template <ByteRange T>
					std::uint32_t calculate_payload_size(const T &bytes, RequestSubTypes sub_type) {
						return calculate_payload_size(get_declared_size(bytes), sub_type);
					}

					template <ByteRange T> std::vector<std::byte> get_payload(const T &bytes) {

						const auto sub_type = std::get<RequestSubTypes>(get_sub_type(bytes));

						if (sub_type == RequestSubTypes::SEE) {
							return See::get_payload(bytes);
						} else if (sub_type == RequestSubTypes::CYCLE) {
							return Cycle::get_payload(bytes);
						} else if (sub_type == RequestSubTypes::CREATE) {
							return Create::get_payload(bytes);
						} else if (sub_type == RequestSubTypes::WANT) {
							return Want::get_payload(bytes);
						} else if (sub_type == RequestSubTypes::ERASE) {
							return Erase::get_payload(bytes);
						}
						Karkinolution::Functional::unreachable_or_throw();
					}

					template <ByteRange T> bool has_payload(const T &bytes) {
						const auto sub_type = std::get<RequestSubTypes>(get_sub_type(bytes));
						if (sub_type == RequestSubTypes::SEE) {
							return See::has_payload(bytes);
						} else if (sub_type == RequestSubTypes::CYCLE) {
							return Cycle::has_payload(bytes);
						} else if (sub_type == RequestSubTypes::CREATE) {
							return Create::has_payload(bytes);
						} else if (sub_type == RequestSubTypes::WANT) {
							return Want::has_payload(bytes);
						} else if (sub_type == RequestSubTypes::ERASE) {
							return Erase::has_payload(bytes);
						}
						Karkinolution::Functional::unreachable_or_throw();
					}

					template <ByteRange T> FrameExistence has_frame(const T &bytes) {
						if (bytes.size() == 0) {
							return FrameExistence::NO;
						}
						if (bytes.size() < ProtocolLayout::SubType::END) {
							return FrameExistence::INCOMPLETE;
						}
						if (get_type(bytes) != BinaryTypes::Request) {
							throw ByteError("Request frame namespace received a non-request frame");
						}

						switch (std::get<RequestSubTypes>(get_sub_type(bytes))) {
						case RequestSubTypes::SEE:
							return See::has_frame(bytes);
						case RequestSubTypes::CREATE:
							return Create::has_frame(bytes);
						case RequestSubTypes::CYCLE:
							return Cycle::has_frame(bytes);
						case RequestSubTypes::ERASE:
							return Erase::has_frame(bytes);
						case RequestSubTypes::WANT:
							return Want::has_frame(bytes);
						}
						Karkinolution::Functional::unreachable_or_throw();
					}

					template <ByteRange T> std::vector<std::byte> get_frame(const T &bytes) {
						switch (std::get<RequestSubTypes>(get_sub_type(bytes))) {
						case RequestSubTypes::SEE:
							return See::get_frame(bytes);
						case RequestSubTypes::CREATE:
							return Create::get_frame(bytes);
						case RequestSubTypes::CYCLE:
							return Cycle::get_frame(bytes);
						case RequestSubTypes::ERASE:
							return Erase::get_frame(bytes);
						case RequestSubTypes::WANT:
							return Want::get_frame(bytes);
						}
						Karkinolution::Functional::unreachable_or_throw();
					}
				} // namespace Request

				namespace Response {
					constexpr std::uint32_t calculate_declared_size(std::uint32_t payload_size) {
						return Internal::calculate_declared_size(
							payload_size,
							ProtocolLayout::Response::Payload::OFFSET);
					}

					constexpr std::uint32_t calculate_payload_size(std::uint32_t declared_size) {
						return Internal::calculate_payload_size(
							declared_size,
							ProtocolLayout::Response::Payload::OFFSET);
					}

					template <ByteRange T>
					constexpr std::uint32_t calculate_payload_size(const T &bytes) {
						return Internal::calculate_payload_size(
							bytes,
							ProtocolLayout::Response::Payload::OFFSET);
					}

					template <ByteRange T> RequestId get_request_id(const T &bytes) {
						return Deserializer::deserialize<RequestId>(
							bytes,
							ProtocolLayout::Response::RequestId::OFFSET);
					}

					template <ByteRange T> bool has_request_id(const T &bytes) {
						return bytes.size() >= ProtocolLayout::Response::RequestId::END;
					}

					template <ByteRange T> ResponseCardinality get_cardinality(const T &bytes) {
						return static_cast<ResponseCardinality>(
							Deserializer::deserialize<Protocol::Aliases::Response::Cardinality>(
								bytes,
								ProtocolLayout::Response::Cardinality::OFFSET));
					}

					template <ByteRange T> bool has_cardinality(const T &bytes) {
						return bytes.size() >= ProtocolLayout::Response::Cardinality::END;
					}

					template <ByteRange T> std::vector<std::byte> get_payload(const T &bytes) {
						std::vector<std::byte> payload;
						payload.insert(payload.end(),
									   bytes.begin() + ProtocolLayout::Response::Payload::OFFSET,
									   bytes.begin()
										   + ProtocolLayout::Response::Payload::OFFSET
										   + Internal::calculate_payload_size(
											   bytes,
											   ProtocolLayout::Response::Payload::OFFSET));
						return payload;
					}

					template <ByteRange T> bool has_payload(const T &bytes) {
						return bytes.size()
							>= ProtocolLayout::Response::Payload::OFFSET
								+ calculate_payload_size(bytes);
					}

					template <ByteRange T> FrameExistence has_frame(const T &bytes) {
						return Internal::has_frame(
							bytes,
							ProtocolLayout::Response::Payload::OFFSET);
					}

					template <ByteRange T> std::vector<std::byte> get_frame(const T &bytes) {
						return Internal::get_frame(
							bytes,
							ProtocolLayout::Response::Payload::OFFSET);
					}
				} // namespace Response

				namespace Error {
					constexpr std::uint32_t calculate_declared_size(std::uint32_t payload_size) {
						return Internal::calculate_declared_size(
							payload_size,
							ProtocolLayout::Error::Payload::OFFSET);
					}

					constexpr std::uint32_t calculate_payload_size(std::uint32_t declared_size) {
						return Internal::calculate_payload_size(
							declared_size,
							ProtocolLayout::Error::Payload::OFFSET);
					}

					template <ByteRange T>
					constexpr std::uint32_t calculate_payload_size(const T &bytes) {
						return Internal::calculate_payload_size(
							bytes,
							ProtocolLayout::Error::Payload::OFFSET);
					}

					template <ByteRange T> RequestId get_request_id(const T &bytes) {
						return Deserializer::deserialize<RequestId>(
							bytes,
							ProtocolLayout::Error::RequestId::OFFSET);
					}

					template <ByteRange T> bool has_request_id(const T &bytes) {
						return bytes.size() >= ProtocolLayout::Error::RequestId::END;
					}

					template <ByteRange T> std::vector<std::byte> get_payload(const T &bytes) {
						std::vector<std::byte> payload;

						payload.insert(payload.end(),
									   bytes.begin() + ProtocolLayout::Error::Payload::OFFSET,
									   bytes.begin() + ProtocolLayout::Error::Payload::OFFSET
										   + Internal::calculate_payload_size(
											   bytes,
											   ProtocolLayout::Error::Payload::OFFSET));
						return payload;
					}

					template <ByteRange T> bool has_payload(const T &bytes) {
						return bytes.size()
							>= ProtocolLayout::Error::Payload::OFFSET + calculate_payload_size(bytes);
					}

					template <ByteRange T> FrameExistence has_frame(const T &bytes) {
						return Internal::has_frame(bytes, ProtocolLayout::Error::Payload::OFFSET);
					}

					template <ByteRange T> std::vector<std::byte> get_frame(const T &bytes) {
						return Internal::get_frame(bytes, ProtocolLayout::Error::Payload::OFFSET);
					}
				} // namespace Error

				template <ByteRange T> FrameExistence has_frame(const T &bytes) {
					if (bytes.size() == 0) {
						return FrameExistence::NO;
					}
					if (bytes.size() < ProtocolLayout::SubType::END) {
						return FrameExistence::INCOMPLETE;
					}

					const auto type = get_type(bytes);
					const auto sub_type = get_sub_type(bytes);
					if (type == BinaryTypes::Request) {
						return Request::has_frame(bytes);
					}
					if (type == BinaryTypes::Response) {
						if (!std::holds_alternative<ResponseSubTypes>(sub_type)) {
							throw ByteError("Response frame has an incompatible subtype");
						}
						return Response::has_frame(bytes);
					}
					if (type == BinaryTypes::Error) {
						if (!std::holds_alternative<ErrorSubTypes>(sub_type)) {
							throw ByteError("Error frame has an incompatible subtype");
						}
						return Error::has_frame(bytes);
					}
					Karkinolution::Functional::unreachable_or_throw();
				}

				// The public dispatchers route by frame type and CodeVariant subtype.
				constexpr std::uint32_t calculate_declared_size(
					std::uint32_t payload_size,
					BinaryTypes   type,
					const Protocol::Detail::CodeVariant &sub_type) {
					if (type == BinaryTypes::Request) {
						if (const auto *request_sub_type =
								std::get_if<RequestSubTypes>(&sub_type)) {
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
					throw ByteError("Frame type and sub type do not match");
				}

				constexpr std::uint32_t calculate_payload_size(
					std::uint32_t declared_size,
					BinaryTypes   type,
					const Protocol::Detail::CodeVariant &sub_type) {
					if (type == BinaryTypes::Request) {
						if (const auto *request_sub_type =
								std::get_if<RequestSubTypes>(&sub_type)) {
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
					throw ByteError("Frame type and sub type do not match");
				}

				template <ByteRange T>
				constexpr std::uint32_t calculate_payload_size(
					const T &bytes,
					BinaryTypes type,
					const Protocol::Detail::CodeVariant &sub_type) {
					return calculate_payload_size(get_declared_size(bytes), type, sub_type);
				}

				template <ByteRange T> std::vector<std::byte> get_payload(const T &bytes) {
					const auto             type = get_type(bytes);
					if (type == BinaryTypes::Request) {
						return Request::get_payload(bytes);
					} else if (type == BinaryTypes::Response) {
						return Response::get_payload(bytes);
					} else if (type == BinaryTypes::Error) {
						return Error::get_payload(bytes);
					}
					Karkinolution::Functional::unreachable_or_throw();
				}

				template <ByteRange T> std::uint32_t get_frame_size(const T &bytes) {
					return ProtocolLayout::DeclaredSize::SIZE + get_declared_size(bytes);
				}

				template <ByteRange T> std::vector<std::byte> get_frame(const T &bytes) {
					if (has_frame(bytes) != FrameExistence::YES) {
						throw ByteError("Cannot extract a frame from an incomplete buffer");
					}

					const auto type = get_type(bytes);
					if (type == BinaryTypes::Request) {
						return Request::get_frame(bytes);
					}
					if (type == BinaryTypes::Response) {
						return Response::get_frame(bytes);
					}
					if (type == BinaryTypes::Error) {
						return Error::get_frame(bytes);
					}
					Karkinolution::Functional::unreachable_or_throw();
				}

				template <ByteRange T>
				Models::Request::Request parse_request_frame(const T &bytes) {
					const auto sub_type = std::get<RequestSubTypes>(get_sub_type(bytes));
					const auto resource = Request::get_resource(bytes);

					Models::Request::RequestExtra extra = std::monostate();

					if (sub_type == Protocol::Request::Operation::SEE) {
						extra =
							Models::Request::SeeExtra{.target = Request::See::get_target(bytes)};
					}
					return Models::Request::Request{.declared_size = get_declared_size(bytes),
													.id       = Request::get_id(bytes),
													.sub_type = sub_type,
													.payload  = get_payload(bytes),
													.resource = resource,
													.extra    = extra};
				}

				template <ByteRange T>
				Models::Response::Response parse_response_frame(const T &bytes) {
					return Models::Response::Response{
						.declared_size = get_declared_size(bytes),
						.request_id    = Response::get_request_id(bytes),
						.sub_type = std::get<ResponseSubTypes>(get_sub_type(bytes)),
						.payload = Response::get_payload(bytes),
						.cardinality = Response::get_cardinality(bytes)};
				}

				template <ByteRange T> Models::Error::Error parse_error_frame(const T &bytes) {
					return Models::Error::Error{.declared_size = get_declared_size(bytes),
												.request_id    = Error::get_request_id(bytes),
												.sub_type = std::get<ErrorSubTypes>(get_sub_type(bytes)),
												.payload       = Error::get_payload(bytes)};
				}

				template <ByteRange T> Models::GenericFrame parse_frame(const T &bytes) {
					const auto type = get_type(bytes);
					Karkinolution::Binary::Protocol::Validator::validate_type(type);
					if (type == BinaryTypes::Request) {
						return Models::GenericFrame{parse_request_frame(bytes), type};
					} else if (type == BinaryTypes::Response) {
						return Models::GenericFrame{parse_response_frame(bytes), type};
					} else if (type == BinaryTypes::Error) {
						return Models::GenericFrame{parse_error_frame(bytes), type};
					}
					Karkinolution::Functional::unreachable_or_throw();
				}
			} // namespace Parser
		} // namespace Frames
	} // namespace Binary
} // namespace Karkinolution
