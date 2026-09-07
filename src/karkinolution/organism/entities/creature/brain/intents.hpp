#pragma once

#include <cstdint>
enum class IntentTypes {
	FIND_FOOD,
	NOTHING
};

struct Intent {
		uint32_t    time;
		IntentTypes type;
};