#pragma once

#include "karkinolution/networking/client_side/interpreters/corpse.hpp"
#include "karkinolution/networking/client_side/interpreters/creature.hpp"
#include "karkinolution/networking/client_side/interpreters/soil.hpp"
#include "karkinolution/networking/client_side/interpreters/territory.hpp"

namespace Karkinolution::Networking::ClientSide::Interpreters::World {
	namespace Corpse    = Interpreters::Corpse;
	namespace Creature  = Interpreters::Creature;
	namespace Soil      = Interpreters::Soil;
	namespace Territory = Interpreters::Territory;
} // namespace Karkinolution::Networking::ClientSide::Interpreters::World

namespace WorldInterpreter = Karkinolution::Networking::ClientSide::Interpreters::World;
