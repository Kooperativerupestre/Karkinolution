#pragma once

#include "karkinolution/networking/frame/core_aliases.hpp"
#include "karkinolution/networking/frame/parser/internal.hpp"
#include "karkinolution/networking/frame/parser/system_output/core.hpp"
#include "shared/functional/unreachable_or_throw.hpp"

#include <karkinolution/binary/byte_range.hpp>
#include <karkinolution/networking/frame/parser/system_output/sub_types/world_state.hpp>

namespace Karkinolution {
	namespace Networking {
		namespace Frames {
			namespace Parser {
				namespace SystemOutput {
					inline constexpr std::uint32_t calculate_declared_size(std::uint32_t        payload_size,
																		   SystemOutputSubTypes sub_type) {
						switch (sub_type) {
							case SystemOutputSubTypes::AllWorldIds:
								return generic_calculate_declared_size(payload_size);
							case SystemOutputSubTypes::WorldState:
								return ST::WorldState::calculate_declared_size(payload_size);
							case SystemOutputSubTypes::None:
								return generic_calculate_declared_size(payload_size);
							case SystemOutputSubTypes::ConfigOutput:
								return generic_calculate_declared_size(payload_size);
						}

						KShared::Functional::unreachable_or_throw("Invalid sub type of SystemOutput (frame)");
					}

					inline std::uint32_t calculate_payload_size(std::uint32_t        declared_size,
																SystemOutputSubTypes sub_type) {
						switch (sub_type) {
							case SystemOutputSubTypes::AllWorldIds:
								return generic_calculate_payload_size(declared_size);
							case SystemOutputSubTypes::WorldState:
								return ST::WorldState::calculate_payload_size(declared_size);
							case SystemOutputSubTypes::None:
								return generic_calculate_payload_size(declared_size);
							case SystemOutputSubTypes::ConfigOutput:
								return generic_calculate_payload_size(declared_size);
						}

						KShared::Functional::unreachable_or_throw("Invalid sub type of SystemOutput (frame)");
					}

					template <Binary::ByteRange T>
					std::uint32_t calculate_payload_size(const T &bytes, SystemOutputSubTypes sub_type) {
						return calculate_payload_size(get_declared_size(bytes), sub_type);
					}

					template <Binary::ByteRange T> std::vector<std::byte> get_payload(const T &bytes) {
						const auto sub_type = std::get<SystemOutputSubTypes>(Internal::get_sub_type(bytes));

						switch (sub_type) {
							case SystemOutputSubTypes::AllWorldIds:
								return generic_get_payload(bytes);
							case SystemOutputSubTypes::WorldState:
								return ST::WorldState::get_payload(bytes);
							case SystemOutputSubTypes::None:
								return generic_get_payload(bytes);
							case SystemOutputSubTypes::ConfigOutput:
								return generic_get_payload(bytes);
						}

						KShared::Functional::unreachable_or_throw("Invalid sub type of SystemOutput (frame)");
					}

				} // namespace SystemOutput
			} // namespace Parser
		} // namespace Frames
	} // namespace Networking
} // namespace Karkinolution