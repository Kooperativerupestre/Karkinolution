#include "karkinolution/networking/frame/world/interpreters/models/soil.hpp"

#include "karkinolution/networking/frame/world/aliases.hpp"
#include "karkinolution/networking/frame/world/contract.hpp"
#include "karkinolution/networking/frame/world/core_aliases.hpp"
#include "karkinolution/terrain/soil/soil.hpp"

#include <karkinolution/api/soil.hpp>
#include <karkinolution/networking/frame/world/interpreters/soil.hpp>
#include <karkinolution/networking/frame/world/motor.hpp>
#include <karkinolution/networking/server_side/api_adapter/soil.hpp>
#include <ranges>
#include <shared/binary/types/list.hpp>

namespace Karkinolution::Networking::ServerSide::APIBYA::Soil {
	ResponseFrame see_soil(const Karkinolution::World::World &world,
						   Frames::World::RequestId           request_id,
						   const std::vector<std::byte>      &payload) {
		const auto id = SoilDSI::Request::interpret_like_get_soil(payload);

		auto soil = Karkinolution::API::Soil::see_soil(world, id);

		if (!soil) {
			return Karkinolution::Networking::Frames::World::Motor::make_none_response(request_id);
		}
		const auto soil_payload = SoilSRI::serialize_soil(*soil);
		return Karkinolution::Networking::Frames::World::Motor::make_response(Frames::World::ResponseSubTypes::SOIL,
																			  request_id,
																			  soil_payload,
																			  Frames::World::ResponseCardinality::ONE);
	}

	ResponseFrame tick_soil(Karkinolution::World::World  &world,
							Frames::World::RequestId      request_id,
							const std::vector<std::byte> &payload) {
		const auto id   = SoilDSI::Request::interpret_like_get_soil(payload);
		const auto soil = Karkinolution::API::Soil::tick(world, id);

		if (!soil) {
			return Karkinolution::Networking::Frames::World::Motor::make_none_response(request_id);
		}

		const auto soil_payload = SoilSRI::serialize_soil(*soil);
		return Karkinolution::Networking::Frames::World::Motor::make_response(Frames::World::ResponseSubTypes::SOIL,
																			  request_id,
																			  soil_payload,
																			  Frames::World::ResponseCardinality::ONE);
	}

	ResponseFrame erase_soil(Karkinolution::World::World  &world,
							 Frames::World::RequestId      request_id,
							 const std::vector<std::byte> &payload) {
		const auto id = SoilDSI::Request::interpret_like_get_soil(payload);
		return Karkinolution::Networking::Frames::World::Motor::make_boolean_response(
			request_id,
			Karkinolution::API::Soil::erase(world, id));
	}

	ResponseFrame see_all_soils(const Karkinolution::World::World &world,
								Frames::World::RequestId           request_id,
								const std::vector<std::byte>      &payload) {
		const auto &all_soils = Karkinolution::API::Soil::see_all_soils(world);

		std::vector<std::byte> r_payload = KShared::Binary::Types::List::SRI::serialize<SoilSRI::Layout::TOTAL_SIZE>(
			std::views::values(all_soils),
			[](const Karkinolution::Terrain::Soil::SoilPiece &soil) {
				return SoilSRI::serialize_soil(soil);
			});

		return Karkinolution::Networking::Frames::World::Motor::make_response(Frames::World::ResponseSubTypes::SOIL,
																			  request_id,
																			  r_payload,
																			  Frames::World::ResponseCardinality::ALL);
	}
} // namespace Karkinolution::Networking::ServerSide::APIBYA::Soil
