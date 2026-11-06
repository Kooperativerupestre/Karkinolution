#pragma once
#include "karkinolution/core/id.hpp"
#include "karkinolution/math/physic/vec/model.hpp"
#include "karkinolution/organism/entities/identity.hpp"
#include "karkinolution/organism/internal_aliases.hpp"

#include <karkinolution/math/units.hpp>
#include <karkinolution/organism/values.hpp>

namespace Karkinolution {
	namespace Organisms {
		namespace Entities {
			namespace Corpse {
				struct Corpse {
						BaseIdType id;
						RawMeat    raw_meat;

						Vec2 position;
						Size size;

						[[nodiscard]] EntityId build_id() const noexcept {
							return create_corpse_id(id);
						}

						Corpse(const Size &size, const Vec2 &position, const RawMeat &raw_meat, BaseIdType id)
							: size(size)
							, position(position)
							, raw_meat(raw_meat)
							, id(id) {}

						Corpse(const Corpse &)            = delete;
						Corpse &operator=(const Corpse &) = delete;

						Corpse(Corpse &&)            = default;
						Corpse &operator=(Corpse &&) = default;

						bool ready_to_disapear() const;
				};

			} // namespace Corpse
		} // namespace Entities
	} // namespace Organisms
} // namespace Karkinolution
