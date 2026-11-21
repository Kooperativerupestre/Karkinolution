#pragma once

#include "karkinolution/binary/frames/models.hpp"
#include "karkinolution/networking/frame/contract.hpp"

#include <chrono>
#include <expected>
#include <future>
#include <karkinolution/binary/core_aliases.hpp>
#include <karkinolution/binary/frames/aliases.hpp>
#include <karkinolution/binary/frames/parser.hpp>
#include <karkinolution/networking/frame/models.hpp>
#include <memory>
#include <mutex>
#include <optional>
#include <unordered_map>

namespace Karkinolution::Networking::ClientSide::Protocol {
	inline std::size_t constexpr RESPONSE_WAITING_TIME_OUT_SECONDS = 5;

	enum class RequestFailure : uint8_t {
		CANCEL,
		DISCONNECTION
	};

	struct CancelledError {};

	using ResponseResult = std::expected<std::optional<ResponseFrameVariant>, RequestFailure>;

	template <typename Response> class FutureResponse {
		public:

			using ResponseResult = std::expected<std::optional<Response>, RequestFailure>;

		private:

			std::optional<std::promise<Response>> promise;
			std::future<Response>                 fut_response;
			std::optional<RequestFailure>         failure;
			mutable std::mutex                    mtx;
			bool                                  wait_in_progress = false;

			bool is_ready_unlocked() const {
				return fut_response.valid()
					&& fut_response.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
			}

			bool can_set_response_unlocked() const {
				if (failure.has_value() || !promise.has_value()) {
					return false;
				}
				return true;
			}

		public:

			FutureResponse()
				: promise(std::in_place)
				, fut_response(promise.value().get_future()) {}

			bool has_response() const {
				std::lock_guard lock{mtx};
				return !failure.has_value() && !wait_in_progress && is_ready_unlocked();
			}

			bool set_response(Response response) {
				std::lock_guard lock{mtx};

				if (!can_set_response_unlocked()) {
					return false;
				}

				promise->set_value(response);
				promise.reset();
				return true;
			}

			bool fail(RequestFailure type) {
				std::lock_guard lock{mtx};

				if (failure.has_value() || !promise.has_value()) {
					return false;
				}
				failure = type;
				promise.reset();
				return true;
			}

			// get instantly the response
			// it only could be blocked by mutex
			ResponseResult try_get() {
				std::lock_guard lock{mtx};

				if (failure.has_value()) {
					return std::unexpected(failure.value());
				}

				if (wait_in_progress || failure.has_value() || !is_ready_unlocked()) {
					return std::optional<ResponseFrameVariant>{};
				}

				return std::optional<ResponseFrameVariant>{fut_response.get()};
			}

			// block the caller thread until response arrives
			ResponseResult await_response(std::chrono::seconds timeout) {
				std::future<ResponseFrameVariant> fut;
				{
					std::lock_guard lock{mtx};

					if (wait_in_progress) {
						return std::optional<ResponseFrameVariant>{};
					}

					if (failure.has_value()) {
						return std::unexpected(failure.value());
					}

					if (is_ready_unlocked()) {
						return std::optional<ResponseFrameVariant>{fut_response.get()};
					}

					wait_in_progress = true;
					fut              = std::move(fut_response);
				}

				const auto status = fut.wait_for(timeout);

				std::lock_guard lock{mtx};
				wait_in_progress = false;

				if (status == std::future_status::ready) {
					if (failure.has_value()) {
						return std::unexpected(failure.value());
					}
					try {
						return std::optional<ResponseFrameVariant>{fut.get()};
					} catch (const std::exception &) {
						return std::unexpected(RequestFailure::DISCONNECTION);
					}
				}

				fut_response = std::move(fut);
				failure      = RequestFailure::CANCEL;
				promise.reset();
				return std::unexpected(RequestFailure::CANCEL);
			}
	};

	class PendingRequests {
		private:

			using SelfFutureResponse = FutureResponse<ResponseFrameVariant>;

		public:

			using ResponseOutput = SelfFutureResponse::ResponseResult;


		private:


			std::mutex                                                                                   mtx;
			std::unordered_map<Binary::RequestId, std::shared_ptr<FutureResponse<ResponseFrameVariant>>> pendings;

			bool erase_request(Binary::RequestId id, const std::shared_ptr<SelfFutureResponse> expected);

		public:

			bool           add_request(Binary::RequestId id);
			bool           add_response(const ResponseFrameVariant &response);
			ResponseResult pop_response(Binary::RequestId id);
			ResponseResult await_response(Binary::RequestId id);
			void           fail_all_by_disconnection();
	};

	using SystemOutput         = Karkinolution::Networking::Frames::Models::SystemOutput::SystemOutput;
	using SystemResponseResult = std::expected<std::optional<SystemOutput>, RequestFailure>;

	class SystemPendingRequests {
		private:

			using SelfFutureResponse = FutureResponse<SystemOutput>;

		public:

			using ResponseResult = SelfFutureResponse::ResponseResult;

		private:

			struct Pending {
					std::promise<SystemOutput>    promise;
					std::future<SystemOutput>     future;
					std::optional<RequestFailure> failure;
			};

			std::mutex                                                           mtx;
			std::unordered_map<Frames::SystemCall::Id, std::shared_ptr<Pending>> promises;

		public:

			bool                 add_request(Frames::SystemCall::Id id);
			bool                 add_response(const SystemOutput &response);
			SystemResponseResult await_response(Frames::SystemCall::Id id);
			void                 fail_all_by_disconnection();
	};
} // namespace Karkinolution::Networking::ClientSide::Protocol
