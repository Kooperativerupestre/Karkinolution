#pragma once

#include "karkinolution/networking/frame/core_aliases.hpp"
#include "karkinolution/networking/frame/parser/internal.hpp"
#include "shared/functional/unreachable_or_throw.hpp"

#include <karkinolution/binary/byte_range.hpp>
#include <karkinolution/networking/frame/parser/system_call/core.hpp>
#include <karkinolution/networking/frame/parser/system_call/sub_types/config.hpp>

namespace Karkinolution {
	namespace Networking {
		namespace Frames {
			namespace Parser {
				namespace SystemCall {
					inline constexpr std::uint32_t calculate_declared_size(std::uint32_t      payload_size,
																		   SystemCallSubTypes sub_type) {
						switch (sub_type) {
							case SystemCallSubTypes::CONFIG:
								return ST::Config::calculate_declared_size(payload_size);
							case SystemCallSubTypes::GET_ALL_WORLD_IDS:
								return generic_calculate_declared_size(payload_size);
						}

						KShared::Functional::unreachable_or_throw("Invalid sub type of SystemCall (frame)");
					}

					inline std::uint32_t calculate_payload_size(std::uint32_t      declared_size,
																SystemCallSubTypes sub_type) {
						switch (sub_type) {
							case SystemCallSubTypes::CONFIG:
								return ST::Config::calculate_payload_size(declared_size);
							case SystemCallSubTypes::GET_ALL_WORLD_IDS:
								return generic_calculate_payload_size(declared_size);
						}
						KShared::Functional::unreachable_or_throw("Invalid sub type of SystemCall (frame)");
					}

					template <Binary::ByteRange T>
					std::uint32_t calculate_payload_size(const T &bytes, SystemCallSubTypes sub_type) {
						return calculate_payload_size(get_declared_size(bytes), sub_type);
					}

					template <Binary::ByteRange T> std::vector<std::byte> get_payload(const T &bytes) {
						const auto sub_type = std::get<SystemCallSubTypes>(Internal::get_sub_type(bytes));

						if (sub_type == SystemCallSubTypes::CONFIG) {
							return ST::Config::get_payload(bytes);
						} else if (sub_type == SystemCallSubTypes::GET_ALL_WORLD_IDS) {
							return generic_get_payload(bytes);
						}
						KShared::Functional::unreachable_or_throw("Invalid sub type of SystemCall (frame))");
					}

				} // namespace SystemCall
			} // namespace Parser
		} // namespace Frames
	} // namespace Networking
} // namespace Karkinolution