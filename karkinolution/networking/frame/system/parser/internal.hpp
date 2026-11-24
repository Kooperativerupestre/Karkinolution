#pragma once
#include "karkinolution/networking/frame/system/core_aliases.hpp"
#include "karkinolution/networking/frame/system/layout.hpp"
#include "shared/binary/byte_range.hpp"
#include "shared/binary/errors.hpp"

#include <cstdint>
#include <karkinolution/networking/frame/system/contract.hpp>
#include <karkinolution/networking/frame/world/parser/internal.hpp>
#include <shared/binary/deserializer.hpp>

namespace Karkinolution {
	namespace Networking {
		namespace Frames::System {
			namespace Parser {
				namespace Internal {

					using FrameExistence = Karkinolution::Networking::Frames::World::Parser::Internal::FrameExistence;

					template <KShared::Binary::ByteRange T> DeclaredSize get_declared_size(const T &bytes) {
						return KShared::Binary::Deserializer::deserialize<DeclaredSize>(bytes,
																						Layout::DeclaredSize::OFFSET);
					}

					template <KShared::Binary::ByteRange T> Type get_type(const T &bytes) {
						return static_cast<Type>(
							KShared::Binary::Deserializer::deserialize<uint8_t>(bytes, Layout::Type::OFFSET));
					}

					template <KShared::Binary::ByteRange T> Detail::SubTypeVariant get_sub_type(const T &bytes) {
						const auto value =
							KShared::Binary::Deserializer::deserialize<uint8_t>(bytes, Layout::SubType::OFFSET);
						if (get_type(bytes) == Type::SystemCall) {
							return static_cast<SystemCallSubTypes>(value);
						}
						return static_cast<SystemOutputSubTypes>(value);
					}

					constexpr std::uint32_t calculate_declared_size(std::uint32_t payload_size,
																	std::size_t   fixed_header_size) {
						return fixed_header_size - Layout::DeclaredSize::SIZE + payload_size;
					}

					constexpr std::uint32_t calculate_payload_size(std::uint32_t declared_size,
																   std::size_t   fixed_header_size) {
						const auto minimum_declared_size = fixed_header_size - Layout::DeclaredSize::SIZE;


						if (declared_size < minimum_declared_size) {
							throw KShared::Binary::ByteError(
								std::format("Cannot get payload bytes because size {} < minimum size {}",
											declared_size,
											minimum_declared_size));
						}
						return declared_size - minimum_declared_size;
					}

					template <KShared::Binary::ByteRange T>
					constexpr std::uint32_t calculate_payload_size(const T &bytes, std::size_t fixed_header_size) {
						const auto declared_size =
							KShared::Binary::Deserializer::read_uint32_t(bytes, Layout::DeclaredSize::OFFSET);


						return calculate_payload_size(declared_size, fixed_header_size);
					}

					template <KShared::Binary::ByteRange T> FrameExistence has_header(const T &bytes) {
						const auto bytes_size = bytes.size();

						if (bytes_size == 0) {
							return FrameExistence::NO;
						}

						if (bytes_size < Layout::DeclaredSize::END) {
							return FrameExistence::DECLARED_SIZE_INCOMPLETE;
						}

						if (bytes_size == Layout::DeclaredSize::END) {
							return FrameExistence::DECLARED_SIZE_COMPLETE;
						}

						if (bytes_size < Layout::Type::END) {
							return FrameExistence::INCOMPLETE;
						}

						if (bytes_size < Layout::SubType::END) {
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

						if (bytes_size >= 0 && bytes_size < Layout::DeclaredSize::END) {
							return FrameExistence::DECLARED_SIZE_INCOMPLETE;
						}

						if (bytes_size == Layout::DeclaredSize::END) {
							return FrameExistence::DECLARED_SIZE_COMPLETE;
						}


						const auto declared_size         = get_declared_size(bytes);
						const auto minimum_declared_size = fixed_header_size - Layout::DeclaredSize::SIZE;
						if (declared_size < minimum_declared_size) {
							throw KShared::Binary::ByteError(
								std::format("Declared size {} is lower than the minimum frame size {}",
											declared_size,
											minimum_declared_size));
						}

						const auto frame_size = Layout::DeclaredSize::SIZE + declared_size;
						return bytes.size() >= frame_size ? FrameExistence::YES : FrameExistence::INCOMPLETE;
					}

				} // namespace Internal
			} // namespace Parser
		} // namespace Frames::System
	} // namespace Networking
} // namespace Karkinolution
