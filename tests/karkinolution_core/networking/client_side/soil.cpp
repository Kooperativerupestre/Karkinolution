#include "karkinolution/networking/client_side/interpreters/soil.hpp"

#include "karkinolution/api/soil.hpp"
#include "karkinolution/networking/client_side/client/client.hpp"
#include "karkinolution/testing/helpers/world.hpp"

#include <gtest/gtest.h>
#include <shared/binary/types/list.hpp>
#include <unordered_set>

using Karkinolution::Networking::Frames::World::RequestId;
using Karkinolution::Networking::Frames::World::ResponseCardinality;
using Karkinolution::Networking::Frames::World::ResponseSubTypes;
using Karkinolution::Terrain::Soil::SoilPiece;
using Karkinolution::World::World;
using KShared::Core::Id::BaseIdType;

namespace {
	class SoilListClient {
		public:

			explicit SoilListClient(const World &world)
				: world(world) {}

			Karkinolution::Networking::ClientSide::Client::WorldResponseResult
			request_world_scope(const Karkinolution::Networking::ClientSide::Client::WorldRequestData &) {
				const auto all_soil_ids = Karkinolution::API::Soil::see_all_soil_ids(world);

				const auto payload = ListTSRI::serialize<sizeof(BaseIdType)>(all_soil_ids, [](BaseIdType id) {
					return KShared::Binary::Serializer::serialize(id);
				});

				const auto response =
					Karkinolution::Networking::Frames::World::Motor::make_response(ResponseSubTypes::SOIL,
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
	const World    world = Karkinolution::Testing::World::make_small_world();
	SoilListClient client{world};

	std::unordered_set<BaseIdType> expected_soil_ids;

	for (const auto &[id, soil] : world.see_territory().registry()) {
		static_cast<void>(soil);
		expected_soil_ids.insert(id);
	}

	const auto soils = SoilInterpreter::see_all_soils(client);

	ASSERT_EQ(soils.size(), expected_soil_ids.size());

	for (const auto id : soils) {
		EXPECT_TRUE(expected_soil_ids.contains(id));
	}
}
