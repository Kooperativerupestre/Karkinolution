#pragma once

#include "karkinolution/networking/frame/contract.hpp"
#include "karkinolution/networking/frame/core_aliases.hpp"
#include "shared/functional/unreachable_or_throw.hpp"

namespace Karkinolution {
	namespace Networking {
		namespace Frames {
			namespace Models {
				namespace SystemCall {
					class SystemCall {
						private:

							DeclaredSize           declared_size;
							std::vector<std::byte> payload;
							Frames::SystemCall::Id id;
							SystemCallSubTypes     sub_type;

						public:

							DeclaredSize get_declared_size() const {
								return declared_size;
							}

							const std::vector<std::byte> &get_payload() const {
								return payload;
							}

							Frames::SystemCall::Id get_id() const {
								return id;
							}

							SystemCallSubTypes get_sub_type() const {
								return sub_type;
							}

							SystemCall(DeclaredSize                 declared_size,
									   const std::vector<std::byte> payload,
									   Frames::SystemCall::Id       id,
									   SystemCallSubTypes           sub_type)
								: declared_size(declared_size)
								, payload(payload)
								, id(id)
								, sub_type(sub_type) {}
					};
				} // namespace SystemCall

				namespace SystemOutput {
					class SystemOutput {
						private:

							DeclaredSize           declared_size;
							Frames::SystemCall::Id call_id;
							std::vector<std::byte> payload;
							SystemOutputSubTypes   sub_type;

						public:


							SystemOutput(DeclaredSize                  declared_size,
										 Frames::SystemCall::Id        call_id,
										 const std::vector<std::byte> &payload,
										 SystemOutputSubTypes          sub_type)
								: declared_size(declared_size)
								, call_id(call_id)
								, payload(payload)
								, sub_type(sub_type) {}

							DeclaredSize get_declared_size() const {
								return declared_size;
							}

							const std::vector<std::byte> &get_payload() const {
								return payload;
							}

							Frames::SystemCall::Id get_call_id() const {
								return call_id;
							}

							SystemOutputSubTypes get_sub_type() const {
								return sub_type;
							}
					};

				} // namespace SystemOutput

				using FramesVariant = std::variant<SystemCall::SystemCall, SystemOutput::SystemOutput>;

				class GenericFrame {
					private:

						FramesVariant data;

					public:

						Type get_type() const {
							if (std::holds_alternative<SystemCall::SystemCall>(data)) {
								return Type::SystemCall;
							} else if (std::holds_alternative<SystemOutput::SystemOutput>(data)) {
								return Type::SystemOutput;
							}
							KShared::Functional::unreachable_or_throw("Invalid type of Frame (system)");
						}

						DeclaredSize get_declared_size() const {
							const auto type = get_type();

							if (type == Type::SystemCall) {
								return std::get<SystemCall::SystemCall>(data).get_declared_size();
							} else if (type == Type::SystemOutput) {
								return std::get<SystemOutput::SystemOutput>(data).get_declared_size();
							}
							KShared::Functional::unreachable_or_throw();
						}

						Detail::SubTypeVariant get_sub_type() const {
							const auto type = get_type();

							if (type == Type::SystemCall) {
								return std::get<SystemCall::SystemCall>(data).get_sub_type();
							} else if (type == Type::SystemOutput) {
								return std::get<SystemOutput::SystemOutput>(data).get_sub_type();
							}
							KShared::Functional::unreachable_or_throw();
						}

						const FramesVariant &get_data() const {
							return data;
						}
				};
			} // namespace Models
		} // namespace Frames
	} // namespace Networking
} // namespace Karkinolution