#pragma once
#include "karkinolution/networking/frame/world/protocol/layouts.hpp"
#include "shared/binary/byte_range.hpp"
#include "shared/binary/errors.hpp"

#include <cstdint>
#include <format>
#include <karkinolution/networking/frame/world/binary_validators.hpp>
#include <karkinolution/networking/frame/world/core_aliases.hpp>
#include <karkinolution/networking/frame/world/invariants.hpp>
#include <shared/binary/deserializer.hpp>
#include <shared/functional/unreachable_or_throw.hpp>

namespace Karkinolution {
	namespace Networking {
		namespace Frames::World {

			namespace Parser {


				namespace Internal {
					constexpr std::uint32_t calculate_declared_size(std::uint32_t payload_size,
																	std::size_t   fixed_header_size) {
						return fixed_header_size - ProtocolLayout::DeclaredSize::SIZE + payload_size;
					}

					constexpr std::uint32_t calculate_payload_size(std::uint32_t declared_size,
																   std::size_t   fixed_header_size) {
						const auto minimum_declared_size = fixed_header_size - ProtocolLayout::DeclaredSize::SIZE;

						if (declared_size < minimum_declared_size) {
							throw KShared::Binary::ByteError(
								std::format("Cannot get payload bytes because size {} < minimum size {}",
											declared_size,
											minimum_declared_size));
						}
						return declared_size - minimum_declared_size;
					}

					template <KShared::Binary::ByteRange T> std::uint32_t get_declared_size(const T &bytes) {
						return KShared::Binary::Deserializer::read_uint32_t(bytes,
																			ProtocolLayout::DeclaredSize::OFFSET);
					}

					template <KShared::Binary::ByteRange T>
					constexpr std::uint32_t calculate_payload_size(const T &bytes, std::size_t fixed_header_size) {
						const auto declared_size = get_declared_size(bytes);
						return calculate_payload_size(declared_size, fixed_header_size);
					}

					template <KShared::Binary::ByteRange T> BinaryTypes get_type(const T &bytes) {
						const auto raw =
							KShared::Binary::Deserializer::read_uint8_t(bytes, ProtocolLayout::Type::OFFSET);

						Protocol::Validator::validate_type(raw);
						return static_cast<BinaryTypes>(raw);
					}

					template <KShared::Binary::ByteRange T>
					Karkinolution::Networking::Frames::World::Protocol::Detail::CodeVariant
					get_sub_type(const T &bytes) {
						Karkinolution::Networking::Frames::World::Protocol::Detail::CodeVariant sub_type;
						const auto                                                              type = get_type(bytes);
						const auto                                                              read =
							KShared::Binary::Deserializer::read_uint8_t(bytes, ProtocolLayout::SubType::OFFSET);

						Karkinolution::Networking::Frames::World::Protocol::Validator::validate_sub_type(type, read);

						if (type == BinaryTypes::Error) {
							return ErrorSubTypes{read};
						} else if (type == BinaryTypes::Request) {
							return RequestSubTypes{read};
						} else if (type == BinaryTypes::Response) {
							return ResponseSubTypes{read};
						}


						KShared::Functional::unreachable_or_throw();
					}

					template <KShared::Binary::ByteRange T> bool has_declared_size(const T &byte) {
						return byte.size() >= ProtocolLayout::DeclaredSize::END;
					}

					template <KShared::Binary::ByteRange T> bool has_type(const T &byte) {
						return byte.size() >= ProtocolLayout::Type::END;
					}

					template <KShared::Binary::ByteRange T> bool has_sub_type(const T &byte) {
						return byte.size() >= ProtocolLayout::SubType::END;
					}

					enum class FrameExistence : uint8_t {
						YES,        // The frame exists completely
						INCOMPLETE, // The declared size exists, additional bytes are present, but the frame is
									// incomplete
						DECLARED_SIZE_COMPLETE,   // Only the declared size is present
						DECLARED_SIZE_INCOMPLETE, // The declared size has started to arrive but is incomplete
						NO                        // Nothing exists
					};

					template <KShared::Binary::ByteRange T> FrameExistence has_header(const T &bytes) {
						const auto bytes_size = bytes.size();

						if (bytes_size == 0) {
							return FrameExistence::NO;
						}

						if (bytes_size < ProtocolLayout::DeclaredSize::END) {
							return FrameExistence::DECLARED_SIZE_INCOMPLETE;
						}

						if (bytes_size == ProtocolLayout::DeclaredSize::END) {
							return FrameExistence::DECLARED_SIZE_COMPLETE;
						}

						if (bytes_size < ProtocolLayout::Type::END) {
							return FrameExistence::INCOMPLETE;
						}

						if (bytes_size < ProtocolLayout::SubType::END) {
							return FrameExistence::INCOMPLETE;
						}

						return FrameExistence::YES;
					}

					template <KShared::Binary::ByteRange T>
					FrameExistence has_frame(const T &bytes, std::size_t fixed_header_size) {
						const auto bytes_size = bytes.size();

						if (bytes_size == 0) {
							return FrameExistence::NO;
						}

						if (bytes_size < ProtocolLayout::DeclaredSize::END) {
							return FrameExistence::DECLARED_SIZE_INCOMPLETE;
						}

						if (bytes_size == ProtocolLayout::DeclaredSize::END) {
							return FrameExistence::DECLARED_SIZE_COMPLETE;
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

						return bytes_size >= frame_size ? FrameExistence::YES : FrameExistence::INCOMPLETE;
					}
				} // namespace Internal


			} // namespace Parser
		} // namespace Frames::World
	} // namespace Networking
} // namespace Karkinolution