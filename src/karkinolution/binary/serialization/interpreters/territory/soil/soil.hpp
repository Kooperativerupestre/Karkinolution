#pragma once
#include "karkinolution/binary/serialization/interpreters/math/geometry/geometry.hpp"
#include "karkinolution/binary/serialization/interpreters/properties/properties.hpp"
#include "karkinolution/math/units.hpp"
#include "karkinolution/terrain/soil.hpp"

#include <cstdint>
#include <karkinolution/binary/fields.hpp>
#include <karkinolution/binary/serialization/interpreters/identity/alias.hpp>

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

					std::byte serialize_type(SoilTypes type);
					std::byte serialize_property(SoilProperties property);

					BytesArray::Properties
					serialize_properties(const std::vector<SoilProperties> &properties);


					BytesArray::RequiredCapabilities serialize_required_capabilities(
						const std::vector<GenericProperty> &capabilities);
					BytesArray::DamageComponent serialize_damage(const SoilPiece &soil);
					BytesArray::MovementCost    serialize_movement_cost(const SoilPiece &soil);
					IdBytes                     serialize_id(const SoilPiece &soil);

					BytesArray::Soil serialize_soil(const SoilPiece &soil);

				} // namespace Soil
			} // namespace SRI
		} // namespace Interpreters
	} // namespace Binary
} // namespace Karkinolution
