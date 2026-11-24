#include <gtest/gtest.h>
#include <karkinolution/networking/frame/world/models.hpp>

namespace {

	namespace Frames = Karkinolution::Networking::Frames::World;
	namespace Models = Frames::Models;
	using Karkinolution::Networking::Frames::World::ErrorSubTypes;
	using Karkinolution::Networking::Frames::World::RequestSubTypes;
	using Karkinolution::Networking::Frames::World::Resource;
	using Karkinolution::Networking::Frames::World::ResponseCardinality;
	using Karkinolution::Networking::Frames::World::ResponseSubTypes;

} // namespace

TEST(FrameModelConstructors, RequestStoresValidatedFields) {
	const std::vector<std::byte>   payload{std::byte{0x2A}};
	const Models::Request::Request request{
		12,
		34,
		RequestSubTypes::SEE,
		payload,
		Resource::CREATURE,
		Models::Request::SeeExtra{.target =
									  Karkinolution::Networking::Frames::World::Protocol::Request::SeeTarget::ONE}};

	EXPECT_EQ(request.get_declared_size(), 12);
	EXPECT_EQ(request.get_id(), 34);
	EXPECT_EQ(request.get_sub_type(), RequestSubTypes::SEE);
	EXPECT_EQ(request.get_payload(), payload);
	EXPECT_EQ(request.get_resource(), Resource::CREATURE);
	ASSERT_TRUE(std::holds_alternative<Models::Request::SeeExtra>(request.get_extra()));

	EXPECT_THROW((Models::Request::Request{12,
										   34,
										   RequestSubTypes::SEE,
										   payload,
										   Resource::CREATURE,
										   Models::Request::WantExtra{}}),
				 Karkinolution::Networking::Frames::World::FrameError);
}

TEST(FrameModelConstructors, TerritorySeeOnlyAcceptsOneTarget) {
	using SeeTarget = Karkinolution::Networking::Frames::World::Protocol::Request::SeeTarget;
	using Request   = Models::Request::Request;
	using SeeExtra  = Models::Request::SeeExtra;

	EXPECT_NO_THROW(
		(Request{12, 34, RequestSubTypes::SEE, {}, Resource::TERRITORY, SeeExtra{.target = SeeTarget::ONE}}));
	EXPECT_THROW((Request{12, 34, RequestSubTypes::SEE, {}, Resource::TERRITORY, SeeExtra{.target = SeeTarget::ALL}}),
				 Karkinolution::Networking::Frames::World::FrameError);

	constexpr Resource other_resources[] = {Resource::CREATURE,
											Resource::EMBRYO,
											Resource::SOIL,
											Resource::CORPSE,
											Resource::EGG};
	for (const auto resource : other_resources) {
		EXPECT_NO_THROW((Request{12, 34, RequestSubTypes::SEE, {}, resource, SeeExtra{.target = SeeTarget::ONE}}));
		EXPECT_NO_THROW((Request{12, 34, RequestSubTypes::SEE, {}, resource, SeeExtra{.target = SeeTarget::ALL}}));
	}

	EXPECT_THROW(
		(Request{12, 34, RequestSubTypes::SEE, {}, Resource::TERRITORY, SeeExtra{.target = static_cast<SeeTarget>(0)}}),
		Karkinolution::Networking::Frames::World::FrameError);
}

TEST(FrameModelConstructors, ResponseStoresValidFieldsAndRejectsNoneWithAll) {
	const std::vector<std::byte>     payload{std::byte{0x2A}};
	const Models::Response::Response response{12, 34, ResponseSubTypes::SOIL, payload, ResponseCardinality::ALL};

	EXPECT_EQ(response.get_declared_size(), 12);
	EXPECT_EQ(response.get_request_id(), 34);
	EXPECT_EQ(response.get_sub_type(), ResponseSubTypes::SOIL);
	EXPECT_EQ(response.get_payload(), payload);
	EXPECT_EQ(response.get_cardinality(), ResponseCardinality::ALL);

	EXPECT_NO_THROW((Models::Response::Response{12, 34, ResponseSubTypes::NONE, payload, ResponseCardinality::ONE}));
	EXPECT_THROW((Models::Response::Response{12, 34, ResponseSubTypes::NONE, payload, ResponseCardinality::ALL}),
				 Karkinolution::Networking::Frames::World::FrameError);
}

TEST(FrameModelConstructors, TerritoryResponseOnlyAcceptsOneCardinality) {
	using Response = Models::Response::Response;

	EXPECT_NO_THROW((Response{12, 34, ResponseSubTypes::TERRITORY, {}, ResponseCardinality::ONE}));
	EXPECT_THROW((Response{12, 34, ResponseSubTypes::TERRITORY, {}, ResponseCardinality::ALL}),
				 Karkinolution::Networking::Frames::World::FrameError);
}

TEST(FrameModelConstructors, ErrorStoresFields) {
	const std::vector<std::byte> payload{std::byte{0x2A}};
	const Models::Error::Error   error{12, 34, ErrorSubTypes::CORPSE_WAS_NOT_FOUND, payload};

	EXPECT_EQ(error.get_declared_size(), 12);
	EXPECT_EQ(error.get_request_id(), 34);
	EXPECT_EQ(error.get_sub_type(), ErrorSubTypes::CORPSE_WAS_NOT_FOUND);
	EXPECT_EQ(error.get_payload(), payload);
}
