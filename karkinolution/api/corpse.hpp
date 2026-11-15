#pragma once

#include "shared/core/id.hpp"

#include <karkinolution/organism/entities/corpse/corpse.hpp>
#include <karkinolution/world/world.hpp>
#include <shared/functional/optional_reference.hpp>

namespace Karkinolution {
	namespace API {
		namespace Corpse {
			using CorpseReference =
				KShared::Functional::OptionalReference<const Karkinolution::Organisms::Entities::Corpse::Corpse>;

			CorpseReference see_corpse(const Karkinolution::World::World &world, KShared::Core::Id::BaseIdType id);
			CorpseReference tick(Karkinolution::World::World &world, KShared::Core::Id::BaseIdType id);
			bool            erase(Karkinolution::World::World &world, KShared::Core::Id::BaseIdType id);
		} // namespace Corpse
	} // namespace API
} // namespace Karkinolution
