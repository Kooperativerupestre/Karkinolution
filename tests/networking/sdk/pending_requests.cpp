#include "karkinolution/binary/core_aliases.hpp"
#include "karkinolution/binary/frames/aliases.hpp"
#include "karkinolution/binary/protocol/contract.hpp"
#include "karkinolution/networking/sdk/src/client/pending_request.hpp"

#include <barrier>
#include <gtest/gtest.h>
#include <karkinolution/binary/frames/motor.hpp>
#include <karkinolution/networking/sdk/src/error.hpp>

using KarkinolutionSDK::Protocol::FutureRequest;
using KarkinolutionSDK::Protocol::PendingRequests;
using KarkinolutionSDK::Protocol::RequestFailure;
using KarkinolutionSDK::Protocol::ResponseResult;

namespace {

	ResponseFrame make_test_response(RequestId id) {
		const std::vector<std::byte> payload{std::byte{0x2A}, std::byte{0x7F}};

		return ResponseFrame{
			static_cast<std::uint32_t>(payload.size()),
			id,
			ResponseSubTypes::CREATURE,
			payload,
			ResponseCardinality::ONE};
	}

} // namespace

TEST(PendingRequests, AddRequestConcurrentlyAcceptsOnlyOne) {
	constexpr RequestId req_id = 4134;

	for (int iteration = 0; iteration < 10000; iteration++) {
		KarkinolutionSDK::Protocol::PendingRequests pending_requests;

		std::barrier start_line(2);

		bool result_a = false;
		bool result_b = false;

		std::thread thread_a([&] {
			start_line.arrive_and_wait();
			result_a = pending_requests.add_request(req_id);
		});

		std::thread thread_b([&] {
			start_line.arrive_and_wait();
			result_b = pending_requests.add_request(req_id);
		});

		thread_a.join();
		thread_b.join();

		EXPECT_EQ(result_a + result_b, 1);
	}
}

TEST(PendingRequests, WaitForTwoOrMoreResponseThrowsAnError) {
	constexpr RequestId req_id = 2384;
}

TEST(PendingRequests, AddResponseConcurrentlyAcceptsOnlyOne) {
	constexpr RequestId    req_id = 2384;
	std::vector<std::byte> payload;
	payload.push_back(static_cast<std::byte>(0x032));

	ResponseFrame response{
		123, req_id, ResponseSubTypes::CORPSE, payload, ResponseCardinality::ONE};

	for (int i = 0; i < 10000; i++) {
		KarkinolutionSDK::Protocol::PendingRequests pending_requests;

		EXPECT_TRUE(pending_requests.add_request(req_id));

		std::barrier start_line(2);

		bool result_a = false;
		bool result_b = false;

		std::thread thread_a([&] {
			start_line.arrive_and_wait();
			result_a = pending_requests.add_response(response);
		});

		std::thread thread_b([&] {
			start_line.arrive_and_wait();
			result_b = pending_requests.add_response(response);
		});

		thread_a.join();
		thread_b.join();

		EXPECT_EQ(result_a + result_b, 1);
	}
}

TEST(PendingRequests, PopResponseThrowsIfRequestDoesNotExist) {
	PendingRequests requests;

	constexpr RequestId id = 42;

	EXPECT_THROW((void)requests.pop_response(id), CPPSdkError);
}

TEST(PendingRequests, PopResponseReturnsEmptyWhenResponseIsNotAvailable) {
	PendingRequests requests;

	constexpr RequestId id = 42;

	ASSERT_TRUE(requests.add_request(id));

	auto result = requests.pop_response(id);

	ASSERT_TRUE(result);
	EXPECT_FALSE(result->has_value());
}

TEST(PendingRequests, PopResponseReturnsAvailableResponse) {
	PendingRequests requests;

	constexpr RequestId id = 42;

	ASSERT_TRUE(requests.add_request(id));

	const auto response = make_test_response(id);

	ASSERT_TRUE(requests.add_response(response));

	auto result = requests.pop_response(id);

	ASSERT_TRUE(result);
	ASSERT_TRUE(result->has_value());
	EXPECT_TRUE(std::holds_alternative<ResponseFrame>(result->value()));
}

TEST(PendingRequests, PopResponseReturnsAvailableErrorFrame) {
	PendingRequests requests;

	constexpr RequestId id = 42;
	const ErrorFrame error{
		123, id, ErrorSubTypes::CREATURE_WAS_NOT_FOUND, std::vector<std::byte>{}};

	ASSERT_TRUE(requests.add_request(id));
	ASSERT_TRUE(requests.add_response(ResponseFrameVariant{error}));

	const auto result = requests.pop_response(id);

	ASSERT_TRUE(result);
	ASSERT_TRUE(result->has_value());
	ASSERT_TRUE(std::holds_alternative<ErrorFrame>(result->value()));
	EXPECT_EQ(std::get<ErrorFrame>(result->value()).sub_type,
			  ErrorSubTypes::CREATURE_WAS_NOT_FOUND);
}

TEST(PendingRequests, PopResponseRemovesRequestAfterRetrievingResponse) {
	PendingRequests requests;

	constexpr RequestId id = 42;

	ASSERT_TRUE(requests.add_request(id));

	const auto response = make_test_response(id);

	ASSERT_TRUE(requests.add_response(response));

	auto result = requests.pop_response(id);

	ASSERT_TRUE(result);
	ASSERT_TRUE(result->has_value());

	EXPECT_THROW((void)requests.pop_response(id), CPPSdkError);
}

TEST(PendingRequests, PopResponseDoesNotRemoveRequestWithoutResponse) {
	PendingRequests requests;

	constexpr RequestId id = 42;

	ASSERT_TRUE(requests.add_request(id));

	auto first_result = requests.pop_response(id);

	ASSERT_TRUE(first_result);
	ASSERT_FALSE(first_result->has_value());

	const auto response = make_test_response(id);

	ASSERT_TRUE(requests.add_response(response));

	auto second_result = requests.pop_response(id);

	ASSERT_TRUE(second_result);
	ASSERT_TRUE(second_result->has_value());
}

TEST(PendingRequests, PopResponseDoesNotRemoveOtherPendingRequests) {
	PendingRequests requests;

	constexpr RequestId first_id  = 42;
	constexpr RequestId second_id = 43;

	ASSERT_TRUE(requests.add_request(first_id));
	ASSERT_TRUE(requests.add_request(second_id));

	const auto response = make_test_response(first_id);

	ASSERT_TRUE(requests.add_response(response));

	auto result = requests.pop_response(first_id);

	ASSERT_TRUE(result);
	ASSERT_TRUE(result->has_value());

	EXPECT_NO_THROW((void)requests.pop_response(second_id));
}

TEST(PendingRequests, FailAllByDisconnectionFailsEveryPendingRequest) {
	PendingRequests requests;

	constexpr RequestId first_id  = 1;
	constexpr RequestId second_id = 2;
	constexpr RequestId third_id  = 3;

	ASSERT_TRUE(requests.add_request(first_id));
	ASSERT_TRUE(requests.add_request(second_id));
	ASSERT_TRUE(requests.add_request(third_id));

	requests.fail_all_by_disconnection();

	for (const auto id : {first_id, second_id, third_id}) {
		auto result = requests.pop_response(id);

		ASSERT_FALSE(result.has_value());
		EXPECT_EQ(result.error(), RequestFailure::DISCONNECTION);
	}
}

TEST(PendingRequests, FailAllByDisconnectionDoesNothingWhenEmpty) {
	PendingRequests requests;

	EXPECT_NO_THROW(requests.fail_all_by_disconnection());
}

TEST(PendingRequests, FailAllByDisconnectionDoesNotRemovePendingRequests) {
	PendingRequests requests;

	constexpr RequestId id = 1;

	ASSERT_TRUE(requests.add_request(id));

	requests.fail_all_by_disconnection();

	// The request must still exist in the map.
	// A second disconnection failure must not cause an invalid-ID error.
	EXPECT_NO_THROW(requests.fail_all_by_disconnection());
}

TEST(PendingRequests, FailAllByDisconnectionDoesNotOverwriteAvailableResponse) {
	PendingRequests requests;

	constexpr RequestId id = 1;

	ASSERT_TRUE(requests.add_request(id));

	const auto response = make_test_response(id);
	ASSERT_TRUE(requests.add_response(response));

	requests.fail_all_by_disconnection();

	auto result = requests.pop_response(id);

	ASSERT_TRUE(result.has_value());
}
