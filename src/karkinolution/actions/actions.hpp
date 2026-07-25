#pragma once
#include <cstdint>



// Removed unused Enum Class Actions

enum class MoveActions : uint8_t  {
    WALK,
    SWIMM,
    MOVE
};

enum IntentActs : uint8_t {
    FIND_FOOD,
    FIND_MATCH,
    NOTHING
};

struct Intent {
    IntentActs act;
    uint16_t time = 0;

    Intent(IntentActs intent): act(intent) {}
};

