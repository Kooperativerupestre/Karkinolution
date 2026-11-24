#pragma once

#include "karkinolution/networking/frame/system/core_aliases.hpp"
#include "karkinolution/networking/frame/system/parser/internal.hpp"
#include "shared/functional/unreachable_or_throw.hpp"

#include <karkinolution/networking/frame/system/parser/system_call/core.hpp>
#include <karkinolution/networking/frame/system/parser/system_call/sub_types/config.hpp>
#include <karkinolution/networking/frame/system/parser/system_call/sub_types/create_world.hpp>
#include <shared/binary/byte_range.hpp>

namespace Karkinolution {
	namespace Networking {
		namespace Frames::System {
			namespace Parser {
				namespace SystemCall {
					inline constexpr std::uint32_t calculate_declared_size(std::uint32_t      payload_size,
																		   SystemCallSubTypes sub_type) {
						switch (sub_type) {
							case SystemCallSubTypes::CONFIG:
								return ST::Config::calculate_declared_size(payload_size);
							case SystemCallSubTypes::GET_ALL_WORLD_IDS:
								return generic_calculate_declared_size(payload_size);
							case SystemCallSubTypes::CREATE_WORLD:
								return ST::CreateWorld::calculate_declared_size(payload_size);
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
							case SystemCallSubTypes::CREATE_WORLD:
								return ST::CreateWorld::calculate_payload_size(declared_size);
						}
						KShared::Functional::unreachable_or_throw("Invalid sub type of SystemCall (frame)");
					}

					template <KShared::Binary::ByteRange T>
					std::uint32_t calculate_payload_size(const T &bytes, SystemCallSubTypes sub_type) {
						return calculate_payload_size(get_declared_size(bytes), sub_type);
					}

					template <KShared::Binary::ByteRange T> std::vector<std::byte> get_payload(const T &bytes) {
						const auto sub_type = std::get<SystemCallSubTypes>(Internal::get_sub_type(bytes));

						if (sub_type == SystemCallSubTypes::CONFIG) {
							return ST::Config::get_payload(bytes);
						} else if (sub_type == SystemCallSubTypes::GET_ALL_WORLD_IDS) {
							return generic_get_payload(bytes);
						} else if (sub_type == SystemCallSubTypes::CREATE_WORLD) {
							return ST::CreateWorld::get_payload(bytes);
						}
						KShared::Functional::unreachable_or_throw("Invalid sub type of SystemCall (frame))");
					}

					template <KShared::Binary::ByteRange T> Internal::FrameExistence has_frame(const T &bytes) {
						const auto sub_type = std::get<SystemCallSubTypes>(Internal::get_sub_type(bytes));

						if (sub_type == SystemCallSubTypes::CONFIG) {
							return ST::Config::has_frame(bytes);
						} else if (sub_type == SystemCallSubTypes::GET_ALL_WORLD_IDS) {
							return generic_has_frame(bytes);
						} else if (sub_type == SystemCallSubTypes::CREATE_WORLD) {
							return ST::CreateWorld::has_frame(bytes);
						}
						KShared::Functional::unreachable_or_throw();
					}

				} // namespace SystemCall
			} // namespace Parser
		} // namespace Frames::System
	} // namespace Networking
} // namespace Karkinolution
