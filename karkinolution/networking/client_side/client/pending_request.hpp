#pragma once

#include "karkinolution/networking/frame/system/contract.hpp"
#include "karkinolution/networking/frame/world/models.hpp"

#include <chrono>
#include <expected>
#include <future>
#include <karkinolution/networking/client_side/error.hpp>
#include <karkinolution/networking/frame/system/models.hpp>
#include <karkinolution/networking/frame/world/aliases.hpp>
#include <karkinolution/networking/frame/world/core_aliases.hpp>
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
					return std::optional<Response>{};
				}

				return ResponseResult{fut_response.get()};
			}

			// block the caller thread until response arrives
			ResponseResult await_response(std::chrono::seconds timeout) {
				std::future<Response> fut;
				{
					std::lock_guard lock{mtx};

					if (wait_in_progress) {
						return std::optional<Response>{};
					}

					if (failure.has_value()) {
						return std::unexpected(failure.value());
					}

					if (is_ready_unlocked()) {
						return std::optional<Response>{fut_response.get()};
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
						return Response{fut.get()};
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

	template <typename Response, typename RequestId> class PendingRequests {
		private:

			using SelfFutureResponse = FutureResponse<Response>;

		public:

			using ResponseResult = SelfFutureResponse::ResponseResult;

		private:

			mutable std::mutex                                                 mtx;
			std::unordered_map<RequestId, std::shared_ptr<SelfFutureResponse>> pendings;

			bool erase_request(RequestId id, const std::shared_ptr<SelfFutureResponse> expected) {
				std::lock_guard lock{mtx};
				const auto      it = pendings.find(id);
				if (it != pendings.end() && it->second == expected) {
					pendings.erase(it);
					return true;
				}
				return false;
			}

		public:

			bool add_request(RequestId id) {
				auto future_response = std::make_shared<SelfFutureResponse>();

				std::lock_guard lock{mtx};
				return pendings.emplace(id, std::move(future_response)).second;
			}

			bool add_response(const Response &response, RequestId request_id) {
				std::shared_ptr<SelfFutureResponse> future_response;
				{
					std::lock_guard lock{mtx};

					auto it = pendings.find(request_id);
					if (it == pendings.end()) {
						throw ClientSideError(
							std::format("Invalid try to add a response to an inexistent request with id = {}",
										request_id));
					}
					future_response = it->second;
				}

				return future_response->set_response(response);
			}

			ResponseResult pop_response(RequestId id) {
				std::shared_ptr<SelfFutureResponse> future_response;
				{
					std::lock_guard lock{mtx};
					auto            it = pendings.find(id);
					if (it == pendings.end()) {
						throw ClientSideError(
							std::format("Invalid pop response of an inexistent request with id = {}", id));
					}
					future_response = it->second;
				}
				auto response = future_response->try_get();

				if (!response || response->has_value()) {
					erase_request(id, future_response);
				}

				return response;
			}

			ResponseResult await_response(RequestId id) {
				std::shared_ptr<SelfFutureResponse> future_response;

				{
					std::lock_guard lock{mtx};

					auto it = pendings.find(id);

					if (it == pendings.end()) {
						throw ClientSideError(
							std::format("Invalid await response of a nonexistent request with id = {}", id));
					}

					future_response = it->second;
				}

				auto result = future_response->await_response(std::chrono::seconds(RESPONSE_WAITING_TIME_OUT_SECONDS));

				erase_request(id, future_response);

				return result;
			}

			void fail_all_by_disconnection() {
				std::vector<std::shared_ptr<SelfFutureResponse>> future_responses;

				{
					std::lock_guard lock{mtx};

					future_responses.reserve(pendings.size());

					for (const auto &[id, request] : pendings) {
						future_responses.push_back(request);
					}
				}

				for (const auto &request : future_responses) {
					request->fail(RequestFailure::DISCONNECTION);
				}
			}

			bool has_request(RequestId id) const {
				std::lock_guard lock{mtx};
				return pendings.contains(id);
			}

			bool has_response(RequestId id) const {
				std::lock_guard lock{mtx};
				if (!pendings.contains(id)) {
					return false;
				}

				const std::shared_ptr<SelfFutureResponse> &future_response = pendings.at(id);

				return future_response->has_response();
			}
	};

} // namespace Karkinolution::Networking::ClientSide::Protocol
