#pragma once
#include "karkinolution/networking/frame/base/parser.hpp"
#include "karkinolution/networking/frame/system/core_aliases.hpp"
#include "karkinolution/networking/frame/system/layout.hpp"
#include "shared/binary/byte_range.hpp"
#include "shared/binary/errors.hpp"

#include <cstdint>
#include <karkinolution/networking/frame/system/contract.hpp>
#include <shared/binary/deserializer.hpp>

namespace Karkinolution {
	namespace Networking {
		namespace Frames::System {
			namespace Parser {
				namespace Internal {

					using FrameExistence = Base::FrameExistence;

					template <KShared::Binary::ByteRange T> DeclaredSize get_declared_size(const T &bytes) {
						return Base::Parser::get_declared_size(bytes);
					}

					template <KShared::Binary::ByteRange T> Type get_type(const T &bytes) {
						return std::get<Type>(Base::Parser::get_type<Type>(bytes));
					}

					template <KShared::Binary::ByteRange T> Detail::SubTypeVariant get_sub_type(const T &bytes) {
						return std::get<Detail::SubTypeVariant>(Base::Parser::get_sub_type<Type>(bytes));
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
						return calculate_payload_size(get_declared_size(bytes), fixed_header_size);
					}

					template <KShared::Binary::ByteRange T> FrameExistence has_header(const T &bytes) {
						return Base::Parser::has_header(bytes);
					}

					template <KShared::Binary::ByteRange T>
					FrameExistence has_frame(const T &bytes, std::size_t fixed_header_size) {
						const auto common = Base::Parser::has_frame(bytes);
						if (common != FrameExistence::YES) {
							return common;
						}
						const auto declared_size         = get_declared_size(bytes);
						const auto minimum_declared_size = fixed_header_size - Layout::DeclaredSize::SIZE;
						if (declared_size < minimum_declared_size) {
							throw KShared::Binary::ByteError(
								std::format("Declared size {} is lower than the minimum frame size {}",
											declared_size,
											minimum_declared_size));
						}

						return FrameExistence::YES;
					}

				} // namespace Internal
			} // namespace Parser
		} // namespace Frames::System
	} // namespace Networking
} // namespace Karkinolution
