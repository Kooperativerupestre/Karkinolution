#pragma once

#include "karkinolution/binary/fields.hpp"
#include "karkinolution/binary/interpreters/geometry.hpp"
#include "karkinolution/binary/interpreters/identity.hpp"
#include "karkinolution/binary/interpreters/models/soil.hpp"
#include "karkinolution/binary/interpreters/properties.hpp"
#include "karkinolution/binary/interpreters/vec.hpp"
#include "karkinolution/core/id.hpp"
#include "karkinolution/math/units.hpp"
#include "karkinolution/organism/entities/properties/properties.hpp"
#include "karkinolution/terrain/soil/soil.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>

namespace Karkinolution {
	namespace Binary {
		namespace Interpreters {
			namespace SRI {
				namespace Soil {
					namespace Layout {
						using Type                      = Field<0, 1>;
						using PropertiesCount           = Next<Type, 1>;
						using PropertiesData            = Next<PropertiesCount, 8>;
						using RequiredCapabilitiesCount = Next<PropertiesData, 1>;
						using RequiredCapabilitiesData  = Next<RequiredCapabilitiesCount, 10>;
						using HasDamage                 = Next<RequiredCapabilitiesData, 1>;
						using Damage                    = Next<HasDamage, 4>;
						using HasMovementCost           = Next<Damage, 1>;
						using MovementCost              = Next<HasMovementCost, 4>;
						using Radius                    = Next<MovementCost, 8>;
						using Position                  = Next<Radius, 24>;
						using Id                        = Next<Position, 8>;

						inline constexpr std::size_t TOTAL_SIZE = Id::END;

						inline constexpr std::size_t MAX_PROPERTIES_SIZE =
							PropertiesCount::SIZE + PropertiesData::SIZE;

						inline constexpr std::size_t MAX_REQUIRED_CAPABILITIES_SIZE =
							RequiredCapabilitiesData::SIZE + RequiredCapabilitiesCount::SIZE;
					} // namespace Layout

					/*
					 * Soil protocol
					 *
					 * [1 byte - type]
					 *
					 * propreties - begin
					 * [1 byte - count]
					 * [8 bytes - properties]
					 * properties - end
					 *
					 * required capabilities - begin
					 * [1 byte - count]
					 * [10 bytes required capabilities]
					 * required capabilities - end
					 *
					 * [1 byte - has damage]
					 * [4 byte - damage value]
					 * [1 byte - has movement cost]
					 * [4 byte - movement cost]
					 *
					 * [8 bytes - radius]
					 * [24 bytes - position]
					 * [8 bytes - id]
					 */

					namespace BytesArray {
						using Properties = std::array<std::byte,
													  Layout::PropertiesCount::SIZE
														  + Layout::PropertiesData::SIZE>;

						using RequiredCapabilities =
							std::array<std::byte,
									   Layout::RequiredCapabilitiesData::SIZE
										   + Layout::RequiredCapabilitiesCount::SIZE>;

						using DamageComponent =
							std::array<std::byte, Layout::Damage::SIZE + Layout::HasDamage::SIZE>;

						using MovementCost =
							std::array<std::byte,
									   Layout::MovementCost::SIZE + Layout::HasMovementCost::SIZE>;

						using Soil = std::array<std::byte, Layout::TOTAL_SIZE>;
					} // namespace BytesArray

					std::byte serialize_type(Karkinolution::Terrain::Soil::SoilTypes type);
					std::byte
					serialize_property(Karkinolution::Terrain::Soil::SoilProperties property);

					BytesArray::Properties serialize_properties(
						const std::vector<Karkinolution::Terrain::Soil::SoilProperties>
							&properties);

					BytesArray::RequiredCapabilities serialize_required_capabilities(
						const std::vector<Karkinolution::Organisms::Entities::GenericProperty>
							&capabilities);
					BytesArray::DamageComponent
					serialize_damage(const Karkinolution::Terrain::Soil::SoilPiece &soil);
					BytesArray::MovementCost
					serialize_movement_cost(const Karkinolution::Terrain::Soil::SoilPiece &soil);
					IdBytes serialize_id(const Karkinolution::Terrain::Soil::SoilPiece &soil);

					BytesArray::Soil
					serialize_soil(const Karkinolution::Terrain::Soil::SoilPiece &soil);

					namespace Request {
						std::array<std::byte, 8>
						make_see_soil(Karkinolution::Core::Id::BaseIdType id);
						std::array<std::byte, 1> make_see_all_soils();
					} // namespace Request

				} // namespace Soil
			} // namespace SRI

			namespace DSI {
				namespace Soil {
					std::optional<Interpreters::Models::Soil::DamageComponent>
					get_damage(const std::vector<std::byte> &payload, std::size_t offset);
					std::optional<Interpreters::Models::Soil::MovementCostComponent>
					get_movement_cost(const std::vector<std::byte> &payload, std::size_t offset);
					Karkinolution::Core::Id::BaseIdType
					get_id(const std::vector<std::byte> &payload, std::size_t offset);
					Karkinolution::Math::Vec::Vec3
					get_position(const std::vector<std::byte> &payload, std::size_t offset);
					Karkinolution::Terrain::Soil::SoilTypes
					get_type(const std::vector<std::byte> &payload, std::size_t offset);
					Karkinolution::Math::Geometry::Radius
					get_radius(const std::vector<std::byte> &payload, std::size_t offset);

					std::vector<Karkinolution::Terrain::Soil::SoilProperties>
					get_soil_properties(const std::vector<std::byte> &payload, std::size_t offset);
					std::vector<Karkinolution::Organisms::Entities::GenericProperty>
					get_required_properties(const std::vector<std::byte> &payload,
											std::size_t                   offset);

					Interpreters::Models::Soil::Soil get_soil(const std::vector<std::byte> &payload,
															  std::size_t                   offset);

					namespace Request {
						Karkinolution::Core::Id::BaseIdType
						interpret_like_get_soil(const std::vector<std::byte> &payload);
					} // namespace Request
				} // namespace Soil
			} // namespace DSI
		} // namespace Interpreters
	} // namespace Binary
} // namespace Karkinolution

namespace SoilSRI = Karkinolution::Binary::Interpreters::SRI::Soil;
namespace SoilDSI = Karkinolution::Binary::Interpreters::DSI::Soil;
