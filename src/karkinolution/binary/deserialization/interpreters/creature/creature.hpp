#pragma once
#include "karkinolution/organism/entities/genetics/genetic.hpp"

#include <karkinolution/core/id.hpp>
#include <vector>

namespace Karkinolution {
	namespace Binary {
		namespace Interpreters {
			namespace DSI {
				namespace Creature {
					namespace Models {
						struct Creature {
								Gender          gender;
								CreatureSpecies specie;
								Vec3            position;
								std::string     name;
								BaseIdType      id;
						};

					} // namespace Models

					Gender get_gender(const std::vector<std::byte> &payload, std::size_t offset);
					CreatureSpecies get_specie(const std::vector<std::byte> &payload,
											   std::size_t                   offset);

					Vec3 get_position(const std::vector<std::byte> &payload, std::size_t offset);

					uint8_t get_name_size(const std::vector<std::byte> &payload,
										  std::size_t                   offset);

					std::string get_name(const std::vector<std::byte> &payload, std::size_t offset);
					BaseIdType  get_id(const std::vector<std::byte> &payload, std::size_t offset);
					Models::Creature get_creature(const std::vector<std::byte> &payload,
												  std::size_t                   offset);

					namespace Request {
						BaseIdType
						interpret_like_get_creature(const std::vector<std::byte> &payload);
					} // namespace Request
				} // namespace Creature
			} // namespace DSI
		} // namespace Interpreters
	} // namespace Binary
} // namespace Karkinolution
