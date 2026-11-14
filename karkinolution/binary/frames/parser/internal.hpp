#pragma once
#include <cstdint>
#include <format>
#include <karkinolution/binary/binary_validators.hpp>
#include <karkinolution/binary/core_aliases.hpp>
#include <karkinolution/binary/frames/invariants.hpp>
#include <shared/binary/deserializer.hpp>
#include <shared/functional/unreachable_or_throw.hpp>

namespace Karkinolution {
	namespace Binary {

		namespace Frames {

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

					template <KShared::Binary::ByteRange T>
					constexpr std::uint32_t calculate_payload_size(const T &bytes, std::size_t fixed_header_size) {
						const auto declared_size =
							KShared::Binary::Deserializer::read_uint32_t(bytes, ProtocolLayout::DeclaredSize::OFFSET);
						return calculate_payload_size(declared_size, fixed_header_size);
					}

					template <KShared::Binary::ByteRange T> std::uint32_t get_declared_size(const T &bytes) {
						return KShared::Binary::Deserializer::read_uint32_t(bytes, 0);
					}

					template <KShared::Binary::ByteRange T> BinaryTypes get_type(const T &bytes) {
						const auto raw =
							KShared::Binary::Deserializer::read_uint8_t(bytes, ProtocolLayout::DeclaredSize::SIZE);

						Protocol::Validator::validate_type(raw);
						return static_cast<BinaryTypes>(raw);
					}

					template <KShared::Binary::ByteRange T>
					Karkinolution::Binary::Protocol::Detail::CodeVariant get_sub_type(const T &bytes) {
						Karkinolution::Binary::Protocol::Detail::CodeVariant sub_type;
						const auto                                           type = get_type(bytes);
						const auto                                           read =
							KShared::Binary::Deserializer::read_uint8_t(bytes, ProtocolLayout::SubType::OFFSET);

						Karkinolution::Binary::Protocol::Validator::validate_sub_type(type, read);

						if (type == BinaryTypes::Error) {
							return ErrorSubTypes{read};
						} else if (type == BinaryTypes::Request) {
							return RequestSubTypes{read};
						} else if (type == BinaryTypes::Response) {
							return ResponseSubTypes{read};
						}


						KShared::Functional::unreachable_or_throw();
					}

					template <KShared::Binary::ByteRange T> bool has_size(const T &byte) {
						return byte.size() >= ProtocolLayout::DeclaredSize::END;
					}

					template <KShared::Binary::ByteRange T> bool has_type(const T &byte) {
						return byte.size() >= ProtocolLayout::Type::END;
					}

					template <KShared::Binary::ByteRange T> bool has_sub_type(const T &byte) {
						return byte.size() >= ProtocolLayout::SubType::END;
					}
				} // namespace Internal

				using Internal::get_declared_size;
				using Internal::get_sub_type;
				using Internal::get_type;
				using Internal::has_size;
				using Internal::has_sub_type;
				using Internal::has_type;
			} // namespace Parser
		} // namespace Frames
	} // namespace Binary
} // namespace Karkinolution