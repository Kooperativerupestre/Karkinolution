#pragma once
#include "karkinolution/networking/frame/base/parser.hpp"
#include "karkinolution/networking/frame/world/layout.hpp"
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

					template <KShared::Binary::ByteRange T> std::uint32_t get_declared_size(const T &bytes) {
						return Base::Parser::get_declared_size(bytes);
					}

					template <KShared::Binary::ByteRange T>
					constexpr std::uint32_t calculate_payload_size(const T &bytes, std::size_t fixed_header_size) {
						const auto declared_size = get_declared_size(bytes);
						return calculate_payload_size(declared_size, fixed_header_size);
					}

					template <KShared::Binary::ByteRange T> BinaryTypes get_type(const T &bytes) {
						const auto type = std::get<BinaryTypes>(Base::Parser::get_type<BinaryTypes>(bytes));
						Validator::validate_type(static_cast<std::uint8_t>(type));
						return type;
					}

					template <KShared::Binary::ByteRange T>
					Karkinolution::Networking::Frames::World::Detail::SubTypeVariant get_sub_type(const T &bytes) {
						const auto sub_type =
							std::get<Detail::SubTypeVariant>(Base::Parser::get_sub_type<BinaryTypes>(bytes));
						const auto type = get_type(bytes);
						const auto read = KShared::Binary::Deserializer::read_uint8_t(bytes, Layout::SubType::OFFSET);

						Karkinolution::Networking::Frames::World::Validator::validate_sub_type(type, read);

						if (type == BinaryTypes::Error) {
							return std::get<ErrorSubTypes>(sub_type);
						} else if (type == BinaryTypes::Request) {
							return std::get<RequestSubTypes>(sub_type);
						} else if (type == BinaryTypes::Response) {
							return std::get<ResponseSubTypes>(sub_type);
						}


						KShared::Functional::unreachable_or_throw();
					}

					template <KShared::Binary::ByteRange T> bool has_declared_size(const T &byte) {
						return byte.size() >= Layout::DeclaredSize::END;
					}

					template <KShared::Binary::ByteRange T> bool has_type(const T &byte) {
						return byte.size() >= Layout::Type::END;
					}

					template <KShared::Binary::ByteRange T> bool has_sub_type(const T &byte) {
						return byte.size() >= Layout::SubType::END;
					}

					using FrameExistence = Base::FrameExistence;

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
		} // namespace Frames::World
	} // namespace Networking
} // namespace Karkinolution
