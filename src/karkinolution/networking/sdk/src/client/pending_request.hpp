#pragma once
#include <chrono>
#include <expected>
#include <future>
#include <karkinolution/binary/core_aliases.hpp>
#include <karkinolution/binary/frames/aliases.hpp>
#include <karkinolution/binary/frames/parser.hpp>
#include <mutex>

namespace KarkinolutionSDK {
	namespace Protocol {
		inline std::size_t constexpr RESPONSE_WAITING_TIME_OUT_SECONDS = 5;

		enum class RequestFailure : uint8_t {
			CANCEL,
			DISCONNECTION
		};

		using ResponseResult = std::expected<std::optional<ResponseFrame>, RequestFailure>;

		class FutureRequest {
			private:

				std::optional<std::promise<ResponseFrame>> promise;
				std::future<ResponseFrame>                 fut_response;
				std::optional<RequestFailure>              failure;
				std::mutex                                 mtx;
				bool                                       wait_in_progress = false;

				bool is_ready_unlocked() const;

			public:


				std::optional<RequestFailure> get_failure();
				FutureRequest();
				bool has_response();
				bool fail(RequestFailure type);
				bool set_response(const ResponseFrame &response);
				// it doesn't wait for the response: If there is, returns, if there isn't, return a
				// nullopt
				ResponseResult try_get();

				// it waits for the response: If it reach the timeout, it returns a nullopt and
				// cancel the request.
				// Only one waiter for request is allowed.
				// If the failure of request is disconnected, it throws an error
				ResponseResult await_response(std::chrono::seconds timeout);
		};

		class PendingRequests {
			private:

				std::mutex                                                    mtx;
				std::unordered_map<RequestId, std::shared_ptr<FutureRequest>> pendings;

				bool erase_request(RequestId id, const std::shared_ptr<FutureRequest> expected);

			public:

				bool           add_request(RequestId id);
				bool           add_response(const ResponseFrame &response);
				ResponseResult pop_response(RequestId id);
				ResponseResult await_response(RequestId id);
				void           fail_all_by_disconnection();
		};
	} // namespace Protocol
} // namespace KarkinolutionSDK