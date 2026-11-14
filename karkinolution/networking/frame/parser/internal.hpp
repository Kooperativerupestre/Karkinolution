#pragma once
#include "karkinolution/binary/byte_range.hpp"
#include "karkinolution/binary/errors.hpp"
#include "karkinolution/networking/frame/layout.hpp"

#include <cstdint>
#include <karkinolution/binary/deserializer.hpp>
#include <karkinolution/networking/frame/contract.hpp>

namespace Karkinolution {
	namespace Networking {
		namespace Frames {
			namespace Parser {
				namespace Internal {

					template <Binary::ByteRange T> DeclaredSize get_declared_size(const T &bytes) {
						return Binary::Deserializer::deserialize<DeclaredSize>(bytes, Layout::DeclaredSize::OFFSET);
					}

					template <Binary::ByteRange T> Type get_type(const T &bytes) {
						return static_cast<Type>(
							Binary::Deserializer::deserialize<uint8_t>(bytes, Layout::Type::OFFSET));
					}

					template <Binary::ByteRange T> Detail::SubTypeVariant get_sub_type(const T &bytes) {
						const auto value = Binary::Deserializer::deserialize<uint8_t>(bytes, Layout::SubType::OFFSET);
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
							throw Binary::ByteError(
								std::format("Cannot get payload bytes because size {} < minimum size {}",
											declared_size,
											minimum_declared_size));
						}
						return declared_size - minimum_declared_size;
					}

					template <Binary::ByteRange T>
					constexpr std::uint32_t calculate_payload_size(const T &bytes, std::size_t fixed_header_size) {
						const auto declared_size =
							Binary::Deserializer::read_uint32_t(bytes, Layout::DeclaredSize::OFFSET);


						return calculate_payload_size(declared_size, fixed_header_size);
					}


				} // namespace Internal
			} // namespace Parser
		} // namespace Frames
	} // namespace Networking
} // namespace Karkinolution
