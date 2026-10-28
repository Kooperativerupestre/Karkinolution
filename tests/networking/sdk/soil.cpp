#include "karkinolution/networking/sdk/src/interpreters/soil.hpp"

#include "karkinolution/networking/sdk/src/client/client.hpp"
#include "karkinolution/test/utils.hpp"

#include <gtest/gtest.h>
#include <karkinolution/binary/types/list.hpp>
#include <unordered_set>

namespace {
	class SoilListClient {
		public:

			explicit SoilListClient(const World &world)
				: world(world) {}

			KarkinolutionSDK::Client::ResponseResult
			request(const KarkinolutionSDK::Client::RequestData &) {
				const auto soil_pieces = world.territory.registry_values();
				const auto payload     = ListTSRI::serialize<SoilSRI::Layout::TOTAL_SIZE>(
                    soil_pieces,
                    [](const SoilPiece &soil) {
                        return SoilSRI::serialize_soil(soil);
                    });
				const auto response =
					Karkinolution::Binary::Frames::Motor::make_response(ResponseSubTypes::SOIL,
																		RequestId{0},
																		payload,
																		ResponseCardinality::ALL);

				return std::optional<ResponseFrameVariant>{response};
			}

		private:

			const World &world;
	};
} // namespace

TEST(SoilSDKInterpreter, ReturnsAllSoils) {
	const World                    world = TestUtils::make_small_world();
	SoilListClient                 client{world};
	std::unordered_set<BaseIdType> expected_soil_ids;
	for (const auto &soil : world.territory.registry_values()) {
		expected_soil_ids.insert(soil.id);
	}

	const auto soils = KarkinolutionSDK::Interpreters::Soil::see_all_soils(client);

	ASSERT_EQ(soils.size(), expected_soil_ids.size());
	for (const auto &soil : soils) {
		EXPECT_TRUE(expected_soil_ids.contains(soil.id));
	}
}
