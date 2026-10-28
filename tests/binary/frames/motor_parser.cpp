#include <array>
#include <cstddef>
#include <gtest/gtest.h>
#include <karkinolution/binary/frames/analyzer.hpp>
#include <karkinolution/binary/frames/motor.hpp>
#include <utility>
#include <vector>

namespace {

	namespace Frames = Karkinolution::Binary::Frames;
	namespace Layout = Karkinolution::Binary::Protocol::Layout;

	constexpr RequestId request_id = 0x0102030405060708ULL;

	constexpr std::array payload = {std::byte{0xA1}, std::byte{0xB2}, std::byte{0xC3}};

	std::vector<std::byte> make_see_request() {
		return Frames::Motor::make_request_raw_bytes(
			RequestSubTypes::SEE,
			request_id,
			Resource::CREATURE,
			payload,
			Frames::Models::Request::SeeExtra{
				.target = Karkinolution::Binary::Protocol::Request::SeeTarget::ALL});
	}

	std::vector<std::byte> make_response() {
		return Frames::Motor::make_response_raw_bytes(
			BinaryTypes::Response,
			std::variant<ErrorSubTypes, ResponseSubTypes>{ResponseSubTypes::SOIL},
			request_id,
			payload,
			ResponseCardinality::ALL);
	}

	std::vector<std::byte> make_error() {
		return Frames::Motor::make_response_raw_bytes(
			BinaryTypes::Error,
			std::variant<ErrorSubTypes, ResponseSubTypes>{ErrorSubTypes::CORPSE_WAS_NOT_FOUND},
			request_id,
			payload);
	}

	template <typename Range> std::vector<std::byte> prefix(const Range &bytes, std::size_t size) {
		return {bytes.begin(), bytes.begin() + size};
	}

} // namespace

TEST(FrameMotorRawBytes, SeeRequestHasReadableMetadataAndPayload) {
	const auto raw = make_see_request();

	ASSERT_TRUE(Frames::Analyzer::is_request(raw));
	EXPECT_FALSE(Frames::Analyzer::is_response(raw));
	EXPECT_FALSE(Frames::Analyzer::is_error(raw));
	EXPECT_EQ(Frames::Parser::has_frame(raw), Frames::Parser::YES);
	EXPECT_EQ(Frames::Parser::get_declared_size(raw) + Layout::DeclaredSize::SIZE, raw.size());
	EXPECT_EQ(Frames::Parser::Request::calculate_payload_size(raw, RequestSubTypes::SEE),
			  payload.size());
	EXPECT_TRUE(Frames::Parser::Request::has_payload(raw));
	EXPECT_EQ(Frames::Parser::get_payload(raw),
			  (std::vector<std::byte>{payload.begin(), payload.end()}));

	const auto parsed = Frames::Parser::parse_request_frame(raw);
	EXPECT_EQ(parsed.get_declared_size(), Frames::Parser::get_declared_size(raw));
	EXPECT_EQ(parsed.get_id(), request_id);
	EXPECT_EQ(parsed.get_sub_type(), RequestSubTypes::SEE);
	EXPECT_EQ(parsed.get_resource(), Resource::CREATURE);
	EXPECT_EQ(parsed.get_payload(), (std::vector<std::byte>{payload.begin(), payload.end()}));
	ASSERT_TRUE(std::holds_alternative<Frames::Models::Request::SeeExtra>(parsed.get_extra()));
	EXPECT_EQ(std::get<Frames::Models::Request::SeeExtra>(parsed.get_extra()).target,
			  Karkinolution::Binary::Protocol::Request::SeeTarget::ALL);

	EXPECT_EQ(Frames::Parser::Request::get_id(raw), request_id);
	EXPECT_EQ(Frames::Parser::Request::get_resource(raw), Resource::CREATURE);
	EXPECT_EQ(Frames::Parser::Request::See::get_target(raw),
			  Karkinolution::Binary::Protocol::Request::SeeTarget::ALL);
}

TEST(FrameMotorRawBytes, NonSeeOperationsPreserveMetadataAndPayload) {
	using RequestExtra          = Frames::Models::Request::RequestExtra;
	const std::array operations = {
		std::pair{RequestSubTypes::CREATE, RequestExtra{Frames::Models::Request::CreateExtra{}}},
		std::pair{RequestSubTypes::CYCLE, RequestExtra{Frames::Models::Request::CycleExtra{}}},
		std::pair{RequestSubTypes::ERASE, RequestExtra{Frames::Models::Request::EraseExtra{}}},
		std::pair{RequestSubTypes::WANT, RequestExtra{Frames::Models::Request::WantExtra{}}},
	};

	for (const auto &[operation, extra] : operations) {
		SCOPED_TRACE(static_cast<int>(operation));
		const auto raw = Frames::Motor::make_request_raw_bytes(operation,
															   request_id,
															   Resource::CORPSE,
															   payload,
															   extra);

		ASSERT_TRUE(Frames::Analyzer::is_request(raw));
		EXPECT_EQ(Frames::Parser::Request::get_id(raw), request_id);
		EXPECT_EQ(Frames::Parser::Request::get_resource(raw), Resource::CORPSE);
		EXPECT_EQ(Frames::Parser::get_payload(raw),
				  (std::vector<std::byte>{payload.begin(), payload.end()}));
		EXPECT_TRUE(Frames::Parser::Request::has_payload(raw));
		EXPECT_EQ(Frames::Parser::has_frame(raw), Frames::Parser::YES);

		const auto parsed = Frames::Parser::parse_request_frame(raw);
		EXPECT_EQ(parsed.get_sub_type(), operation);
		EXPECT_EQ(parsed.get_id(), request_id);
		EXPECT_EQ(parsed.get_resource(), Resource::CORPSE);
		EXPECT_EQ(parsed.get_payload(), (std::vector<std::byte>{payload.begin(), payload.end()}));
		if (operation == RequestSubTypes::WANT) {
			EXPECT_TRUE(
				std::holds_alternative<Frames::Models::Request::WantExtra>(parsed.get_extra()));
		} else if (operation == RequestSubTypes::CREATE) {
			EXPECT_TRUE(
				std::holds_alternative<Frames::Models::Request::CreateExtra>(parsed.get_extra()));
		} else if (operation == RequestSubTypes::CYCLE) {
			EXPECT_TRUE(
				std::holds_alternative<Frames::Models::Request::CycleExtra>(parsed.get_extra()));
		} else {
			EXPECT_TRUE(
				std::holds_alternative<Frames::Models::Request::EraseExtra>(parsed.get_extra()));
		}
	}
}

TEST(FrameMotorRawBytes, ResponseIsParsedWithCardinalityAndPayload) {
	const auto raw = make_response();

	ASSERT_TRUE(Frames::Analyzer::is_response(raw));
	EXPECT_FALSE(Frames::Analyzer::is_request(raw));
	EXPECT_FALSE(Frames::Analyzer::is_error(raw));
	EXPECT_EQ(Frames::Parser::has_frame(raw), Frames::Parser::YES);
	EXPECT_EQ(Frames::Parser::get_declared_size(raw) + Layout::DeclaredSize::SIZE, raw.size());
	EXPECT_TRUE(Frames::Parser::Response::has_request_id(raw));
	EXPECT_TRUE(Frames::Parser::Response::has_cardinality(raw));
	EXPECT_TRUE(Frames::Parser::Response::has_payload(raw));
	EXPECT_EQ(Frames::Parser::Response::get_payload(raw),
			  (std::vector<std::byte>{payload.begin(), payload.end()}));

	const auto parsed = Frames::Parser::parse_response_frame(raw);
	EXPECT_EQ(parsed.get_request_id(), request_id);
	EXPECT_EQ(parsed.get_sub_type(), ResponseSubTypes::SOIL);
	EXPECT_EQ(parsed.get_cardinality(), ResponseCardinality::ALL);
	EXPECT_EQ(parsed.get_payload(), (std::vector<std::byte>{payload.begin(), payload.end()}));
}

TEST(FrameMotorRawBytes, ErrorIsParsedWithoutResponseCardinality) {
	const auto raw = make_error();

	ASSERT_TRUE(Frames::Analyzer::is_error(raw));
	EXPECT_FALSE(Frames::Analyzer::is_request(raw));
	EXPECT_FALSE(Frames::Analyzer::is_response(raw));
	EXPECT_EQ(Frames::Parser::has_frame(raw), Frames::Parser::YES);
	EXPECT_EQ(Frames::Parser::get_declared_size(raw) + Layout::DeclaredSize::SIZE, raw.size());
	EXPECT_TRUE(Frames::Parser::Error::has_request_id(raw));
	EXPECT_TRUE(Frames::Parser::Error::has_payload(raw));
	EXPECT_EQ(Frames::Parser::Error::get_payload(raw),
			  (std::vector<std::byte>{payload.begin(), payload.end()}));

	const auto parsed = Frames::Parser::parse_error_frame(raw);
	EXPECT_EQ(parsed.get_request_id(), request_id);
	EXPECT_EQ(parsed.get_sub_type(), ErrorSubTypes::CORPSE_WAS_NOT_FOUND);
	EXPECT_EQ(parsed.get_payload(), (std::vector<std::byte>{payload.begin(), payload.end()}));
}

TEST(FrameParserHas, RequestPredicatesRespectEachFieldBoundary) {
	const auto raw = make_see_request();

	for (std::size_t size = 0; size <= raw.size(); ++size) {
		const auto bytes = prefix(raw, size);
		SCOPED_TRACE(size);

		EXPECT_EQ(Frames::Parser::has_size(bytes), size >= Layout::DeclaredSize::END);
		EXPECT_EQ(Frames::Parser::has_type(bytes), size >= Layout::Type::END);
		EXPECT_EQ(Frames::Parser::has_sub_type(bytes), size >= Layout::SubType::END);
		EXPECT_EQ(Frames::Parser::Request::has_id(bytes), size >= Layout::Request::Id::END);
		EXPECT_EQ(Frames::Parser::Request::has_resource(bytes),
				  size >= Layout::Request::Resource::END);
		EXPECT_EQ(Frames::Parser::Request::See::has_target(bytes),
				  size >= Layout::Request::See::Target::END);

		const auto expected_frame_state = size == 0
			? Frames::Parser::NO
			: (size < raw.size() ? Frames::Parser::INCOMPLETE : Frames::Parser::YES);
		EXPECT_EQ(Frames::Parser::has_frame(bytes), expected_frame_state);
		if (size >= Layout::SubType::END) {
			EXPECT_EQ(Frames::Parser::Request::has_payload(bytes), size == raw.size());
		}
	}
}

TEST(FrameParserHas, ResponseAndErrorPredicatesRespectTheirFieldBoundaries) {
	const auto response = make_response();
	const auto error    = make_error();

	for (std::size_t size = 0; size <= response.size(); ++size) {
		const auto bytes = prefix(response, size);
		SCOPED_TRACE(::testing::Message() << "response prefix " << size);

		EXPECT_EQ(Frames::Parser::Response::has_request_id(bytes),
				  size >= Layout::Response::RequestId::END);
		EXPECT_EQ(Frames::Parser::Response::has_cardinality(bytes),
				  size >= Layout::Response::Cardinality::END);
		EXPECT_EQ(Frames::Parser::has_frame(bytes),
				  size == 0 ? Frames::Parser::NO
							: (size < response.size() ? Frames::Parser::INCOMPLETE
													  : Frames::Parser::YES));
		if (size >= Layout::DeclaredSize::END) {
			EXPECT_EQ(Frames::Parser::Response::has_payload(bytes), size == response.size());
		}
	}

	for (std::size_t size = 0; size <= error.size(); ++size) {
		const auto bytes = prefix(error, size);
		SCOPED_TRACE(::testing::Message() << "error prefix " << size);

		EXPECT_EQ(Frames::Parser::Error::has_request_id(bytes),
				  size >= Layout::Error::RequestId::END);
		EXPECT_EQ(Frames::Parser::has_frame(bytes),
				  size == 0
					  ? Frames::Parser::NO
					  : (size < error.size() ? Frames::Parser::INCOMPLETE : Frames::Parser::YES));
		if (size >= Layout::DeclaredSize::END) {
			EXPECT_EQ(Frames::Parser::Error::has_payload(bytes), size == error.size());
		}
	}
}

TEST(FrameParser, ExtractsOneCompleteFrameFromTrailingBytes) {
	const auto first    = make_response();
	const auto second   = make_error();
	auto       combined = first;
	combined.insert(combined.end(), second.begin(), second.end());

	EXPECT_TRUE(Frames::Analyzer::is_complete_frame(first));
	EXPECT_FALSE(Frames::Analyzer::is_complete_frame(combined));
	EXPECT_EQ(Frames::Parser::has_frame(combined), Frames::Parser::YES);
	EXPECT_EQ(Frames::Parser::get_frame(combined), first);
}

TEST(FrameMotorRawBytes, RejectsSubtypeFromTheWrongFrameKind) {
	EXPECT_THROW(
		Frames::Motor::make_response_raw_bytes(
			BinaryTypes::Response,
			std::variant<ErrorSubTypes, ResponseSubTypes>{ErrorSubTypes::CORPSE_WAS_NOT_FOUND},
			request_id,
			payload),
		FrameInvariantError);
	EXPECT_THROW(Frames::Motor::make_response_raw_bytes(
					 BinaryTypes::Error,
					 std::variant<ErrorSubTypes, ResponseSubTypes>{ResponseSubTypes::SOIL},
					 request_id,
					 payload),
				 FrameInvariantError);
}
