#pragma once
#include <future>
#include <karkinolution/binary/core_aliases.hpp>
#include <karkinolution/binary/frames/aliases.hpp>
#include <karkinolution/binary/frames/parser.hpp>
#include <mutex>

namespace KarkinolutionSDK {
	namespace Protocol {
		inline std::size_t constexpr RESPONSE_WAITING_TIME_OUT_SECONDS = 5;

		class PendingRequests {
			private:

				std::mutex                                                 mtx;
				std::unordered_map<RequestId, std::future<ResponseFrame>>  futures;
				std::unordered_map<RequestId, std::promise<ResponseFrame>> pendings;

			public:

				bool add_request(RequestId id);
				bool add_response(const ResponseFrame &response);
				void cancel(RequestId id);
				// it doesn't wait for the response: If there is, returns, if there isn't, return a
				// nullopt
				std::optional<ResponseFrame> pop_response(RequestId id);

				// it waits for the response: If it reach the timeout, it returns a nullopt and
				// cancel the request
				std::optional<ResponseFrame> await_response(RequestId id);
				ResponseFrame                get_or_throw(std::future<ResponseFrame> &fut);
				void                         faill_all();
		};
	} // namespace Protocol
} // namespace KarkinolutionSDK