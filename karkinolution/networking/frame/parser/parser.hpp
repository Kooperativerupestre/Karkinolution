#pragma once
#include "karkinolution/binary/byte_range.hpp"
#include "karkinolution/networking/frame/contract.hpp"
#include "karkinolution/networking/frame/core_aliases.hpp"
#include "karkinolution/networking/frame/layout.hpp"
#include "karkinolution/networking/frame/parser/internal.hpp"
#include "karkinolution/networking/frame/parser/system_call/core.hpp"
#include "karkinolution/networking/frame/parser/system_output/core.hpp"
#include "shared/functional/unreachable_or_throw.hpp"

#include <karkinolution/networking/frame/models.hpp>
#include <karkinolution/networking/frame/parser/system_call/parser.hpp>
#include <karkinolution/networking/frame/parser/system_output/parser.hpp>

namespace Karkinolution {
	namespace Networking {
		namespace Frames {
			namespace Parser {
				enum class FrameExistence : uint8_t {
					YES,
					INCOMPLETE,
					NO
				};

				template <Binary::ByteRange T> FrameExistence has_frame(const T &bytes, std::size_t fixed_header_size) {
					if (bytes.size() == 0) {
						return FrameExistence::NO;
					}
					if (bytes.size() < Layout::DeclaredSize::END) {
						return FrameExistence::INCOMPLETE;
					}
					const auto declared_size         = Internal::get_declared_size(bytes);
					const auto minimum_declared_size = fixed_header_size - Layout::DeclaredSize::SIZE;
					if (declared_size < minimum_declared_size) {
						throw Binary::ByteError(std::format("Declared size {} is lower than the minimum frame size {}",
															declared_size,
															minimum_declared_size));
					}
					const auto frame_size = Layout::DeclaredSize::SIZE + declared_size;
					return bytes.size() >= frame_size ? FrameExistence::YES : FrameExistence::INCOMPLETE;
				}

				template <Binary::ByteRange T> std::vector<std::byte> get_frame(const T &bytes) {
					const auto frame_size = Layout::DeclaredSize::SIZE + Internal::get_declared_size(bytes);

					std::vector<std::byte> frame;
					frame.reserve(frame_size);
					for (std::size_t i = 0; i < frame_size; ++i) {
						frame.push_back(bytes[i]);
					}
					return frame;
				}

				constexpr std::uint32_t
				calculate_declared_size(std::uint32_t payload_size, Type type, const Detail::SubTypeVariant &sub_type) {
					if (type == Type::SystemCall) {
						if (const auto* system_call_sub_type = std::get_if<SystemCallSubTypes>(&sub_type)) {
							return SystemCall::calculate_declared_size(payload_size, *system_call_sub_type);
						}
					} else if (type == Type::SystemOutput) {
						if (const auto* system_output_sub_type = std::get_if<SystemOutputSubTypes>(&sub_type)) {
							return SystemOutput::calculate_declared_size(payload_size, *system_output_sub_type);
						}
					}
					KShared::Functional::unreachable_or_throw("Frame type and sub type do not match");
				}

				constexpr std::uint32_t
				calculate_payload_size(std::uint32_t declared_size, Type type, const Detail::SubTypeVariant &sub_type) {
					if (type == Type::SystemCall) {
						if (const auto* system_call_sub_type = std::get_if<SystemCallSubTypes>(&sub_type)) {
							return SystemCall::calculate_payload_size(declared_size, *system_call_sub_type);
						}
					} else if (type == Type::SystemOutput) {
						if (const auto* system_output_sub_type = std::get_if<SystemOutputSubTypes>(&sub_type)) {
							return SystemOutput::calculate_payload_size(declared_size, *system_output_sub_type);
						}
					}
					KShared::Functional::unreachable_or_throw("Frame type and sub type do not match");
				}

				template <Binary::ByteRange T>
				constexpr std::uint32_t
				calculate_payload_size(const T &bytes, Type type, const Detail::SubTypeVariant &sub_type) {
					return calculate_payload_size(Internal::get_declared_size(bytes), type, sub_type);
				}

				template <Binary::ByteRange T> std::vector<std::byte> get_payload(const T &bytes) {
					const auto type = Internal::get_type(bytes);

					if (type == Type::SystemCall) {
						return SystemCall::get_payload(bytes);
					} else if (type == Type::SystemOutput) {
						return SystemOutput::get_payload(bytes);
					}
					KShared::Functional::unreachable_or_throw("Invalid type of Frame (system)");
				}

				template <Binary::ByteRange T> constexpr std::uint32_t get_frame_size(const T &bytes) {
					return Layout::DeclaredSize::SIZE + Internal::get_declared_size(bytes);
				}

				template <Binary::ByteRange T> Models::SystemCall::SystemCall parse_system_call_frame(const T &bytes) {
					const auto sub_type = std::get<SystemCallSubTypes>(Internal::get_sub_type(bytes));

					return {Internal::get_declared_size(bytes),
							get_payload(bytes),
							SystemCall::get_id(bytes),
							sub_type};
				}

				template <Binary::ByteRange T>
				Models::SystemOutput::SystemOutput parse_system_output_frame(const T &bytes) {
					const auto sub_type = std::get<SystemOutputSubTypes>(Internal::get_sub_type(bytes));

					return {Internal::get_declared_size(bytes),
											SystemOutput::get_id(bytes),
											get_payload(bytes),
							sub_type};
				}
			} // namespace Parser
		} // namespace Frames
	} // namespace Networking
} // namespace Karkinolution
