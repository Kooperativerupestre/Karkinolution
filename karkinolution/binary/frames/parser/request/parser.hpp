#pragma once
#include <karkinolution/binary/frames/parser/internal.hpp>
#include <karkinolution/binary/frames/parser/request/core.hpp>
#include <karkinolution/binary/frames/parser/request/sub_types/create.hpp>
#include <karkinolution/binary/frames/parser/request/sub_types/cycle.hpp>
#include <karkinolution/binary/frames/parser/request/sub_types/erase.hpp>
#include <karkinolution/binary/frames/parser/request/sub_types/see.hpp>
#include <karkinolution/binary/frames/parser/request/sub_types/want.hpp>

namespace Karkinolution {
	namespace Binary {

		namespace Frames {

			namespace Parser {


				namespace Request {

					namespace See    = ST::See;
					namespace Create = ST::Create;
					namespace Cycle  = ST::Cycle;
					namespace Erase  = ST::Erase;
					namespace Want   = ST::Want;

					inline std::uint32_t calculate_declared_size(std::uint32_t payload_size, RequestSubTypes sub_type) {
						switch (sub_type) {
							case RequestSubTypes::SEE:
								return ST::See::calculate_declared_size(payload_size);
							case RequestSubTypes::CREATE:
								return ST::Create::calculate_declared_size(payload_size);
							case RequestSubTypes::CYCLE:
								return ST::Cycle::calculate_declared_size(payload_size);
							case RequestSubTypes::ERASE:
								return ST::Erase::calculate_declared_size(payload_size);
							case RequestSubTypes::WANT:
								return ST::Want::calculate_declared_size(payload_size);
						}
						KShared::Functional::unreachable_or_throw();
					}

					inline std::uint32_t calculate_payload_size(std::uint32_t declared_size, RequestSubTypes sub_type) {
						switch (sub_type) {
							case RequestSubTypes::SEE:
								return ST::See::calculate_payload_size(declared_size);
							case RequestSubTypes::CREATE:
								return ST::Create::calculate_payload_size(declared_size);
							case RequestSubTypes::CYCLE:
								return ST::Cycle::calculate_payload_size(declared_size);
							case RequestSubTypes::ERASE:
								return ST::Erase::calculate_payload_size(declared_size);
							case RequestSubTypes::WANT:
								return ST::Want::calculate_payload_size(declared_size);
						}
						KShared::Functional::unreachable_or_throw();
					}

					template <ByteRange T>
					std::uint32_t calculate_payload_size(const T &bytes, RequestSubTypes sub_type) {
						return calculate_payload_size(get_declared_size(bytes), sub_type);
					}

					template <ByteRange T> std::vector<std::byte> get_payload(const T &bytes) {

						const auto sub_type = std::get<RequestSubTypes>(get_sub_type(bytes));

						if (sub_type == RequestSubTypes::SEE) {
							return ST::See::get_payload(bytes);
						} else if (sub_type == RequestSubTypes::CYCLE) {
							return ST::Cycle::get_payload(bytes);
						} else if (sub_type == RequestSubTypes::CREATE) {
							return ST::Create::get_payload(bytes);
						} else if (sub_type == RequestSubTypes::WANT) {
							return ST::Want::get_payload(bytes);
						} else if (sub_type == RequestSubTypes::ERASE) {
							return ST::Erase::get_payload(bytes);
						}
						KShared::Functional::unreachable_or_throw();
					}


				} // namespace Request
			} // namespace Parser
		} // namespace Frames
	} // namespace Binary
} // namespace Karkinolution
