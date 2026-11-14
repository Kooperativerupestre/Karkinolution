#pragma once

#include "karkinolution/binary/byte_range.hpp"
#include "karkinolution/binary/byte_utils.hpp"
#include "karkinolution/binary/serializer.hpp"
#include "karkinolution/networking/frame/contract.hpp"
#include "karkinolution/networking/frame/core_aliases.hpp"
#include "karkinolution/networking/frame/layout.hpp"
#include "karkinolution/networking/frame/models.hpp"
#include "karkinolution/networking/frame/parser/parser.hpp"
#include "karkinolution/networking/frame/utils.hpp"

namespace Karkinolution {
	namespace Networking {
		namespace Frames {
			namespace Motor {
				template <Binary::ByteRange Payload> std::vector<std::byte> copy_payload(const Payload &payload) {
					std::vector<std::byte> copied_payload;
					copied_payload.reserve(payload.size());
					for (std::size_t index = 0; index < payload.size(); ++index) {
						copied_payload.push_back(payload[index]);
					}
					return copied_payload;
				}

				void constexpr append_declared_size(std::vector<std::byte> &frame, DeclaredSize declared_size) {
					Binary::Utils::append_bytes(frame, Binary::Serializer::serialize(declared_size));
				}

				void constexpr append_type(std::vector<std::byte> &frame, Type type) {
					Binary::Utils::append_bytes(frame, Binary::Serializer::serialize(static_cast<uint8_t>(type)));
				}

				void constexpr append_sub_type(std::vector<std::byte> &frame, Detail::SubTypeVariant sub_type) {
					Binary::Utils::append_bytes(
						frame,
						Binary::Serializer::serialize(static_cast<uint8_t>(Utils::cast_numeric(sub_type))));
				}

				template <Binary::ByteRange T>
				Models::SystemOutput::SystemOutput
				make_system_output(SystemOutputSubTypes sub_type, SystemCall::Id call_id, const T &payload) {
					return {Parser::calculate_declared_size(payload.size(), Type::SystemOutput, sub_type),
							call_id,
							copy_payload(payload),
							sub_type};
				}

				template <Binary::ByteRange T>
				Models::SystemCall::SystemCall
				make_system_call(SystemCallSubTypes sub_type, SystemCall::Id id, const T &payload) {
					return {Parser::calculate_declared_size(payload.size(), Type::SystemCall, sub_type),
							id,
							copy_payload(payload),
							sub_type};
				}

				Models::SystemOutput::SystemOutput make_none_system_output(SystemCall::Id call_id);

				template <Binary::ByteRange T>
				std::vector<std::byte>
				make_raw_system_output(SystemOutputSubTypes sub_type, SystemCall::Id call_id, const T &payload) {
					std::vector<std::byte> frame;

					const auto declared_size =
						Parser::calculate_declared_size(payload.size(), Type::SystemOutput, sub_type);

					frame.reserve(declared_size + Layout::DeclaredSize::SIZE);

					// declared size

					append_declared_size(frame, declared_size);

					// type

					append_type(frame, Type::SystemOutput);

					// sub type

					append_sub_type(frame, sub_type);

					// call_id

					Binary::Utils::append_bytes(frame, Binary::Serializer::serialize(call_id));
					frame.insert(frame.end(), payload.begin(), payload.end());
					return frame;
				}

				template <Binary::ByteRange T>
				std::vector<std::byte>
				make_raw_system_call(SystemCallSubTypes sub_type, SystemCall::Id id, const T &payload) {
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

					Binary::Utils::append_bytes(frame, Binary::Serializer::serialize(id));
					frame.insert(frame.end(), payload.begin(), payload.end());
					return frame;
				}


			} // namespace Motor
		} // namespace Frames
	} // namespace Networking
} // namespace Karkinolution
