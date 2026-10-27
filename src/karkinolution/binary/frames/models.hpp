#pragma once
#include "karkinolution/binary/frames/error.hpp"
#include "karkinolution/binary/protocol/aliases.hpp"
#include "karkinolution/binary/protocol/contract.hpp"
#include "karkinolution/binary/protocol/layouts.hpp"
#include "karkinolution/core/error.hpp"
#include "karkinolution/functional/unreachable_or_throw.hpp"

#include <karkinolution/binary/byte_range.hpp>
#include <karkinolution/binary/core_aliases.hpp>
#include <utility>
#include <vector>

namespace Karkinolution {
	namespace Binary {
		namespace Frames {
			namespace Models {

				namespace Request {
					struct SeeExtra {
							Protocol::Request::SeeTarget target;
					};

					struct WantExtra {};

					struct EraseExtra {};

					struct CreateExtra {};

					struct CycleExtra {};

					using RequestExtra = std::variant<SeeExtra,
													  WantExtra,
													  EraseExtra,
													  CreateExtra,
													  CycleExtra,
													  std::monostate>;

					void constexpr validate_sub_type_and_extra(RequestSubTypes     sub_type,
															   const RequestExtra &extra) {
						if (sub_type == RequestSubTypes::SEE) {
							if (!std::holds_alternative<SeeExtra>(extra)) {
								throw FrameError("Request with sub type See must have SeeExtra");
							}
						} else if (sub_type == RequestSubTypes::WANT) {
							if (!std::holds_alternative<WantExtra>(extra)) {
								throw FrameError("Request with sub type Want must have WantExtra");
							}
						} else if (sub_type == RequestSubTypes::CREATE) {
							if (!std::holds_alternative<CreateExtra>(extra)) {
								throw FrameError(
									"Request with sub type Create must have CreateExtra");
							}
						} else if (sub_type == RequestSubTypes::CYCLE) {
							if (!std::holds_alternative<CycleExtra>(extra)) {
								throw FrameError(
									"Request with sub type Cycle must have CycleExtra");
							}
						} else if (sub_type == RequestSubTypes::ERASE) {
							if (!std::holds_alternative<EraseExtra>(extra)) {
								throw FrameError(
									"Request with sub type Erase must have EraseExtra");
							}
						} else {
							Karkinolution::Functional::unreachable_or_throw();
						}
					}

					struct Request {
							FrameDeclaredSize      declared_size;
							RequestId              id;
							RequestSubTypes        sub_type;
							std::vector<std::byte> payload;
							Resource               resource;

							RequestExtra extra;
					};

				} // namespace Request

				namespace Response {
					struct Response {
							FrameDeclaredSize      declared_size;
							RequestId              request_id;
							ResponseSubTypes       sub_type;
							std::vector<std::byte> payload;
							ResponseCardinality    cardinality;
					};
				} // namespace Response

				namespace Error {
					struct Error {
							FrameDeclaredSize      declared_size;
							RequestId              request_id;
							ErrorSubTypes          sub_type;
							std::vector<std::byte> payload;
					};
				} // namespace Error

				class GenericFrame {
					private:

						std::variant<Request::Request, Response::Response, Error::Error> data_;

					public:

						GenericFrame(
							const std::variant<Request::Request, Response::Response, Error::Error>
								&data)
							: data_(data) {}

						BinaryTypes constexpr get_type() const {
							if (std::holds_alternative<Request::Request>(data_)) {
								return BinaryTypes::Request;
							} else if (std::holds_alternative<Response::Response>(data_)) {
								return BinaryTypes::Response;
							} else if (std::holds_alternative<Error::Error>(data_)) {
								return BinaryTypes::Error;
							}
							Karkinolution::Functional::unreachable_or_throw();
						}

						FrameDeclaredSize constexpr get_declared_size() const {
							const auto type = get_type();
							if (type == BinaryTypes::Request) {
								return std::get<Request::Request>(data_).declared_size;
							} else if (type == BinaryTypes::Response) {
								return std::get<Response::Response>(data_).declared_size;
							} else if (type == BinaryTypes::Error) {
								return std::get<Error::Error>(data_).declared_size;
							}
							Karkinolution::Functional::unreachable_or_throw();
						}

						Protocol::Detail::CodeVariant constexpr get_sub_type() const {
							const auto type = get_type();
							if (type == BinaryTypes::Request) {
								return std::get<Request::Request>(data_).sub_type;
							} else if (type == BinaryTypes::Response) {
								return std::get<Response::Response>(data_).sub_type;
							} else if (type == BinaryTypes::Error) {
								return std::get<Error::Error>(data_).sub_type;
							}
							Karkinolution::Functional::unreachable_or_throw();
						}

						const auto constexpr &get_data() const {
							return data_;
						}
				};

			} // namespace Models
		} // namespace Frames
	} // namespace Binary
} // namespace Karkinolution
