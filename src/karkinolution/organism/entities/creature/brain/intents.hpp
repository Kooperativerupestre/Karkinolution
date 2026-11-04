#pragma once

#include <cstdint>

namespace Karkinolution::Organisms::Entities::Creature::Brain::Intents {

	enum class IntentTypes {
		FIND_FOOD,
		NOTHING
	};

	struct Intent {
			uint32_t    time;
			IntentTypes type;
	};

} // namespace Karkinolution::Organisms::Entities::Creature::Brain::Intents
