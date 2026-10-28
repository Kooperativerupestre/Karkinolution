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

				// Frame models enforce protocol invariants once at construction instead of
				// repeating checks in the parser and motor. Private fields and read-only
				// accessors prevent later mutation and keep those layers consistent.
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
						private:

							FrameDeclaredSize      declared_size_;
							RequestId              id_;
							RequestSubTypes        sub_type_;
							std::vector<std::byte> payload_;
							Resource               resource_;
							RequestExtra           extra_;

						public:

							Request(FrameDeclaredSize      declared_size,
									RequestId              id,
									RequestSubTypes        sub_type,
									std::vector<std::byte> payload,
									Resource               resource,
									RequestExtra           extra)
								: declared_size_(declared_size)
								, id_(id)
								, sub_type_(sub_type)
								, payload_(std::move(payload))
								, resource_(resource)
								, extra_(std::move(extra)) {
								validate_sub_type_and_extra(sub_type_, extra_);
							}

							Request(const Request &)            = default;
							Request(Request &&)                 = default;
							Request &operator=(const Request &) = delete;
							Request &operator=(Request &&)      = delete;

							FrameDeclaredSize get_declared_size() const {
								return declared_size_;
							}

							RequestId get_id() const {
								return id_;
							}

							RequestSubTypes get_sub_type() const {
								return sub_type_;
							}

							const std::vector<std::byte> &get_payload() const {
								return payload_;
							}

							Resource get_resource() const {
								return resource_;
							}

							const RequestExtra &get_extra() const {
								return extra_;
							}
					};

				} // namespace Request

				namespace Response {
					void constexpr validate_sub_type_and_cardinality(
						ResponseSubTypes    sub_type,
						ResponseCardinality cardinality) {
						if (sub_type == ResponseSubTypes::NONE) {
							if (cardinality != ResponseCardinality::ONE) {
								throw FrameError(
									"Response with sub type None must have cardinality One");
							}
						} else if (sub_type != ResponseSubTypes::CREATURE
								   && sub_type != ResponseSubTypes::CORPSE
								   && sub_type != ResponseSubTypes::SOIL) {
							throw FrameError("Invalid response subtype");
						}

						if (cardinality != ResponseCardinality::ONE
							&& cardinality != ResponseCardinality::ALL) {
							throw FrameError("Invalid response cardinality");
						}
					}

					struct Response {
						private:

							FrameDeclaredSize      declared_size_;
							RequestId              request_id_;
							ResponseSubTypes       sub_type_;
							std::vector<std::byte> payload_;
							ResponseCardinality    cardinality_;

						public:

							Response(FrameDeclaredSize      declared_size,
									 RequestId              request_id,
									 ResponseSubTypes       sub_type,
									 std::vector<std::byte> payload,
									 ResponseCardinality    cardinality)
								: declared_size_(declared_size)
								, request_id_(request_id)
								, sub_type_(sub_type)
								, payload_(std::move(payload))
								, cardinality_(cardinality) {
								validate_sub_type_and_cardinality(sub_type_, cardinality_);
							}

							Response(const Response &)            = default;
							Response(Response &&)                 = default;
							Response &operator=(const Response &) = delete;
							Response &operator=(Response &&)      = delete;

							FrameDeclaredSize get_declared_size() const {
								return declared_size_;
							}

							RequestId get_request_id() const {
								return request_id_;
							}

							ResponseSubTypes get_sub_type() const {
								return sub_type_;
							}

							const std::vector<std::byte> &get_payload() const {
								return payload_;
							}

							ResponseCardinality get_cardinality() const {
								return cardinality_;
							}
					};
				} // namespace Response

				namespace Error {
					struct Error {
						private:

							FrameDeclaredSize      declared_size_;
							RequestId              request_id_;
							ErrorSubTypes          sub_type_;
							std::vector<std::byte> payload_;

						public:

							Error(FrameDeclaredSize      declared_size,
								  RequestId              request_id,
								  ErrorSubTypes          sub_type,
								  std::vector<std::byte> payload)
								: declared_size_(declared_size)
								, request_id_(request_id)
								, sub_type_(sub_type)
								, payload_(std::move(payload)) {}

							Error(const Error &)            = default;
							Error(Error &&)                 = default;
							Error &operator=(const Error &) = delete;
							Error &operator=(Error &&)      = delete;

							FrameDeclaredSize get_declared_size() const {
								return declared_size_;
							}

							RequestId get_request_id() const {
								return request_id_;
							}

							ErrorSubTypes get_sub_type() const {
								return sub_type_;
							}

							const std::vector<std::byte> &get_payload() const {
								return payload_;
							}
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
								return std::get<Request::Request>(data_).get_declared_size();
							} else if (type == BinaryTypes::Response) {
								return std::get<Response::Response>(data_).get_declared_size();
							} else if (type == BinaryTypes::Error) {
								return std::get<Error::Error>(data_).get_declared_size();
							}
							Karkinolution::Functional::unreachable_or_throw();
						}

						Protocol::Detail::CodeVariant constexpr get_sub_type() const {
							const auto type = get_type();
							if (type == BinaryTypes::Request) {
								return std::get<Request::Request>(data_).get_sub_type();
							} else if (type == BinaryTypes::Response) {
								return std::get<Response::Response>(data_).get_sub_type();
							} else if (type == BinaryTypes::Error) {
								return std::get<Error::Error>(data_).get_sub_type();
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
