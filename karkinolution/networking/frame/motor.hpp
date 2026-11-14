#pragma once

#include "karkinolution/networking/frame/contract.hpp"
#include "karkinolution/networking/frame/core_aliases.hpp"
#include "karkinolution/networking/frame/layout.hpp"
#include "karkinolution/networking/frame/models.hpp"
#include "karkinolution/networking/frame/parser/parser.hpp"
#include "karkinolution/networking/frame/utils.hpp"
#include "shared/binary/byte_range.hpp"
#include "shared/binary/byte_utils.hpp"
#include "shared/binary/serializer.hpp"

namespace Karkinolution {
	namespace Networking {
		namespace Frames {
			namespace Motor {
				template <KShared::Binary::ByteRange Payload>
				std::vector<std::byte> copy_payload(const Payload &payload) {
					std::vector<std::byte> copied_payload;
					copied_payload.reserve(payload.size());
					for (std::size_t index = 0; index < payload.size(); ++index) {
						copied_payload.push_back(payload[index]);
					}
					return copied_payload;
				}

				void constexpr append_declared_size(std::vector<std::byte> &frame, DeclaredSize declared_size) {
					KShared::Binary::Utils::append_bytes(frame, KShared::Binary::Serializer::serialize(declared_size));
				}

				void constexpr append_type(std::vector<std::byte> &frame, Type type) {
					KShared::Binary::Utils::append_bytes(
						frame,
						KShared::Binary::Serializer::serialize(static_cast<uint8_t>(type)));
				}

				void constexpr append_sub_type(std::vector<std::byte> &frame, Detail::SubTypeVariant sub_type) {
					KShared::Binary::Utils::append_bytes(
						frame,
						KShared::Binary::Serializer::serialize(static_cast<uint8_t>(Utils::cast_numeric(sub_type))));
				}

				template <KShared::Binary::ByteRange T>
				Models::SystemOutput::SystemOutput
				make_system_output(SystemOutputSubTypes sub_type, SystemCall::Id call_id, const T &payload) {
					return {Parser::calculate_declared_size(
								payload.size(),
								Type::SystemOutput,
								Detail::SubTypeVariant{std::in_place_type<SystemOutputSubTypes>, sub_type}),
							call_id,
							copy_payload(payload),
							sub_type};
				}

				template <KShared::Binary::ByteRange T>
				Models::SystemCall::SystemCall
				make_system_call(SystemCallSubTypes                         sub_type,
								 SystemCall::Id                             id,
								 const T                                   &payload,
								 const Models::SystemCall::SystemCallExtra &extra = std::monostate{}) {
					return {Parser::calculate_declared_size(payload.size(), Type::SystemCall, sub_type),
							copy_payload(payload),
							id,
							sub_type,
							extra};
				}

				Models::SystemOutput::SystemOutput make_none_system_output(SystemCall::Id call_id);

				template <KShared::Binary::ByteRange T>
				std::vector<std::byte>
				make_raw_system_output(SystemOutputSubTypes sub_type, SystemCall::Id call_id, const T &payload) {
					std::vector<std::byte> frame;

					const auto declared_size = Parser::calculate_declared_size(
						payload.size(),
						Type::SystemOutput,
						Detail::SubTypeVariant{std::in_place_type<SystemOutputSubTypes>, sub_type});

					frame.reserve(declared_size + Layout::DeclaredSize::SIZE);

					// declared size

					append_declared_size(frame, declared_size);

					// type

					append_type(frame, Type::SystemOutput);

					// sub type

					append_sub_type(frame, sub_type);

					// call_id

					KShared::Binary::Utils::append_bytes(frame, KShared::Binary::Serializer::serialize(call_id));
					frame.insert(frame.end(), payload.begin(), payload.end());
					return frame;
				}

				template <KShared::Binary::ByteRange T>
				std::vector<std::byte>
				make_raw_system_call(SystemCallSubTypes                         sub_type,
									 SystemCall::Id                             id,
									 const T                                   &payload,
									 const Models::SystemCall::SystemCallExtra &extra = std::monostate{}) {
					std::vector<std::byte> frame;

					const auto declared_size =
						Parser::calculate_declared_size(payload.size(), Type::SystemCall, sub_type);

					frame.reserve(declared_size + Layout::DeclaredSize::SIZE);

					// declared size

					append_declared_size(frame, declared_size);

					// type

					append_type(frame, Type::SystemCall);

					// sub type

					append_sub_type(frame, sub_type);

					// id

					KShared::Binary::Utils::append_bytes(frame, KShared::Binary::Serializer::serialize(id));
					if (sub_type == SystemCallSubTypes::CREATE_WORLD) {
						Models::SystemCall::SystemCall{declared_size, copy_payload(payload), id, sub_type, extra};
						KShared::Binary::Utils::append_bytes(
							frame,
							KShared::Binary::Serializer::serialize(
								std::get<Models::SystemCall::CreateWorldExtra>(extra).auto_connect));
					}
					frame.insert(frame.end(), payload.begin(), payload.end());
					return frame;
				}


			} // namespace Motor
		} // namespace Frames
	} // namespace Networking
} // namespace Karkinolution
