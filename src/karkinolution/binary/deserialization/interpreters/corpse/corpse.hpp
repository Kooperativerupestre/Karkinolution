#pragma once

#include <cstddef>
#include <cstdint>
#include <karkinolution/core/id.hpp>
#include <karkinolution/math/physic/vec/model.hpp>
#include <karkinolution/math/units.hpp>
#include <karkinolution/organism/stats.hpp>
#include <vector>

namespace Karkinolution {
	namespace Binary {
		namespace Interpreters {
			namespace DSI {
				namespace Corpse {
					namespace Models {

						struct Corpse {
								BaseIdType                   id;
								OrganismStats::Body::RawMeat raw_meat;
								Vec3                         position;
								Size                         size;
						};
					} // namespace Models

					BaseIdType get_id(const std::vector<std::byte> &payload, std::size_t offset);
					OrganismStats::Body::RawMeat get_raw_meat(const std::vector<std::byte> &payload,
															  std::size_t                   offset);
					Vec3 get_position(const std::vector<std::byte> &payload, std::size_t offset);
					Size get_size(const std::vector<std::byte> &payload, std::size_t offset);
					Models::Corpse get_corpse(const std::vector<std::byte> &payload,
											  std::size_t                   offset);

					namespace Request {
						BaseIdType interpret_like_see_corpse(const std::vector<std::byte> &payload);
					} // namespace Request
				} // namespace Corpse
			} // namespace DSI
		} // namespace Interpreters
	} // namespace Binary
} // namespace Karkinolution
