#include "karkinolution/networking/client_side/interpreters/soil.hpp"

#include "karkinolution/networking/client_side/client/client.hpp"
#include "karkinolution/test/utils.hpp"

#include <gtest/gtest.h>
#include <karkinolution/binary/types/list.hpp>
#include <ranges>
#include <unordered_set>

using Karkinolution::Binary::RequestId;
using Karkinolution::Binary::ResponseCardinality;
using Karkinolution::Binary::ResponseSubTypes;
using Karkinolution::Terrain::Soil::SoilPiece;
using Karkinolution::World::World;
using KShared::Core::Id::BaseIdType;

namespace {
	class SoilListClient {
		public:

			explicit SoilListClient(const World &world)
				: world(world) {}

			Karkinolution::Networking::ClientSide::Client::ResponseResult
			request(const Karkinolution::Networking::ClientSide::Client::RequestData &) {
				const auto &soil_pieces = world.see_territory().registry();
				const auto  payload =
					ListTSRI::serialize<SoilSRI::Layout::TOTAL_SIZE>(std::views::values(soil_pieces),
																	 [](const SoilPiece &soil) {
																		 return SoilSRI::serialize_soil(soil);
																	 });
				const auto response = Karkinolution::Binary::Frames::Motor::make_response(ResponseSubTypes::SOIL,
																						  RequestId{0},
																						  payload,
																						  ResponseCardinality::ALL);

				return std::optional<ResponseFrameVariant>{response};
			}

		private:

			const World &world;
	};
} // namespace

TEST(SoilInterpreter, ReturnsAllSoils) {
	const World                    world = TestUtils::make_small_world();
	SoilListClient                 client{world};
	std::unordered_set<BaseIdType> expected_soil_ids;
	for (const auto &soil : std::views::values(world.see_territory().registry())) {
		expected_soil_ids.insert(soil.id);
	}

	const auto soils = SoilInterpreter::see_all_soils(client);

	ASSERT_EQ(soils.size(), expected_soil_ids.size());
	for (const auto &soil : soils) {
		EXPECT_TRUE(expected_soil_ids.contains(soil.id));
	}
}
