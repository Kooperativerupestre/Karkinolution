#include "karkinolution/networking/client_side/client/pending_request.hpp"
#include "karkinolution/networking/client_side/error.hpp"
#include "karkinolution/networking/frame/world/aliases.hpp"
#include "karkinolution/networking/frame/world/core_aliases.hpp"

#include <barrier>
#include <gtest/gtest.h>
#include <karkinolution/networking/frame/world/motor.hpp>

using Karkinolution::Networking::ClientSide::ClientSideError;
using Karkinolution::Networking::ClientSide::Protocol::FutureResponse;
using Karkinolution::Networking::ClientSide::Protocol::PendingRequests;
using Karkinolution::Networking::ClientSide::Protocol::RequestFailure;
using Karkinolution::Networking::Frames::World::ErrorSubTypes;
using Karkinolution::Networking::Frames::World::RequestId;
using Karkinolution::Networking::Frames::World::ResponseCardinality;
using Karkinolution::Networking::Frames::World::ResponseSubTypes;

namespace {

	ResponseFrame make_test_response(RequestId id) {
		const std::vector<std::byte> payload{std::byte{0x2A}, std::byte{0x7F}};

		return ResponseFrame{static_cast<std::uint32_t>(payload.size()),
							 id,
							 ResponseSubTypes::CREATURE,
							 payload,
							 ResponseCardinality::ONE};
	}

} // namespace

TEST(PendingRequests, AddRequestConcurrentlyAcceptsOnlyOne) {
	constexpr RequestId req_id = 4134;

	for (int iteration = 0; iteration < 10000; iteration++) {
		PendingRequests<ResponseFrameVariant, RequestId> pending_requests;

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

	ResponseFrame response{123, req_id, ResponseSubTypes::CORPSE, payload, ResponseCardinality::ONE};

	for (int i = 0; i < 10000; i++) {
		Karkinolution::Networking::ClientSide::Protocol::PendingRequests<ResponseFrameVariant, RequestId>
			pending_requests;

		EXPECT_TRUE(pending_requests.add_request(req_id));

		std::barrier start_line(2);

		bool result_a = false;
		bool result_b = false;

		std::thread thread_a([&] {
			start_line.arrive_and_wait();
			result_a = pending_requests.add_response(response, response.get_request_id());
		});

		std::thread thread_b([&] {
			start_line.arrive_and_wait();
			result_b = pending_requests.add_response(response, response.get_request_id());
		});

		thread_a.join();
		thread_b.join();

		EXPECT_EQ(result_a + result_b, 1);
	}
}

TEST(PendingRequests, PopResponseThrowsIfRequestDoesNotExist) {
	PendingRequests<ResponseFrameVariant, RequestId> requests;

	constexpr RequestId id = 42;

	EXPECT_THROW((void)requests.pop_response(id), ClientSideError);
}

TEST(PendingRequests, PopResponseReturnsEmptyWhenResponseIsNotAvailable) {
	PendingRequests<ResponseFrameVariant, RequestId> requests;

	constexpr RequestId id = 42;

	ASSERT_TRUE(requests.add_request(id));

	auto result = requests.pop_response(id);

	ASSERT_TRUE(result);
	EXPECT_FALSE(result->has_value());
}

TEST(PendingRequests, PopResponseReturnsAvailableResponse) {
	PendingRequests<ResponseFrameVariant, RequestId> requests;

	constexpr RequestId id = 42;

	ASSERT_TRUE(requests.add_request(id));

	const auto response = make_test_response(id);

	ASSERT_TRUE(requests.add_response(response, response.get_request_id()));

	auto result = requests.pop_response(id);

	ASSERT_TRUE(result);
	ASSERT_TRUE(result->has_value());
	EXPECT_TRUE(std::holds_alternative<ResponseFrame>(result->value()));
}

TEST(PendingRequests, PopResponseReturnsAvailableErrorFrame) {
	PendingRequests<ResponseFrameVariant, RequestId> requests;

	constexpr RequestId id = 42;
	const ErrorFrame    error{123, id, ErrorSubTypes::CREATURE_WAS_NOT_FOUND, std::vector<std::byte>{}};

	ASSERT_TRUE(requests.add_request(id));
	ASSERT_TRUE(requests.add_response(ResponseFrameVariant{error}, id));

	const auto result = requests.pop_response(id);

	ASSERT_TRUE(result);
	ASSERT_TRUE(result->has_value());
	ASSERT_TRUE(std::holds_alternative<ErrorFrame>(result->value()));
	EXPECT_EQ(std::get<ErrorFrame>(result->value()).get_sub_type(), ErrorSubTypes::CREATURE_WAS_NOT_FOUND);
}

TEST(PendingRequests, PopResponseRemovesRequestAfterRetrievingResponse) {
	PendingRequests<ResponseFrameVariant, RequestId> requests;

	constexpr RequestId id = 42;

	ASSERT_TRUE(requests.add_request(id));

	const auto response = make_test_response(id);

	ASSERT_TRUE(requests.add_response(response, id));

	auto result = requests.pop_response(id);

	ASSERT_TRUE(result);
	ASSERT_TRUE(result->has_value());

	EXPECT_THROW((void)requests.pop_response(id), ClientSideError);
}

TEST(PendingRequests, PopResponseDoesNotRemoveRequestWithoutResponse) {
	PendingRequests<ResponseFrameVariant, RequestId> requests;

	constexpr RequestId id = 42;

	ASSERT_TRUE(requests.add_request(id));

	auto first_result = requests.pop_response(id);

	ASSERT_TRUE(first_result);
	ASSERT_FALSE(first_result->has_value());

	const auto response = make_test_response(id);

	ASSERT_TRUE(requests.add_response(response, id));

	auto second_result = requests.pop_response(id);

	ASSERT_TRUE(second_result);
	ASSERT_TRUE(second_result->has_value());
}

TEST(PendingRequests, PopResponseDoesNotRemoveOtherPendingRequests) {
	PendingRequests<ResponseFrameVariant, RequestId> requests;

	constexpr RequestId first_id  = 42;
	constexpr RequestId second_id = 43;

	ASSERT_TRUE(requests.add_request(first_id));
	ASSERT_TRUE(requests.add_request(second_id));

	const auto response = make_test_response(first_id);

	ASSERT_TRUE(requests.add_response(response, first_id));

	auto result = requests.pop_response(first_id);

	ASSERT_TRUE(result);
	ASSERT_TRUE(result->has_value());

	EXPECT_NO_THROW((void)requests.pop_response(second_id));
}

TEST(PendingRequests, FailAllByDisconnectionFailsEveryPendingRequest) {
	PendingRequests<ResponseFrameVariant, RequestId> requests;

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
	PendingRequests<ResponseFrameVariant, RequestId> requests;

	EXPECT_NO_THROW(requests.fail_all_by_disconnection());
}

TEST(PendingRequests, FailAllByDisconnectionDoesNotRemovePendingRequests) {
	PendingRequests<ResponseFrameVariant, RequestId> requests;

	constexpr RequestId id = 1;

	ASSERT_TRUE(requests.add_request(id));

	requests.fail_all_by_disconnection();

	// The request must still exist in the map.
	// A second disconnection failure must not cause an invalid-ID error.
	EXPECT_NO_THROW(requests.fail_all_by_disconnection());
}

TEST(PendingRequests, FailAllByDisconnectionDoesNotOverwriteAvailableResponse) {
	PendingRequests<ResponseFrameVariant, RequestId> requests;

	constexpr RequestId id = 1;

	ASSERT_TRUE(requests.add_request(id));

	const auto response = make_test_response(id);
	ASSERT_TRUE(requests.add_response(response, id));

	requests.fail_all_by_disconnection();

	auto result = requests.pop_response(id);

	ASSERT_TRUE(result.has_value());
}
