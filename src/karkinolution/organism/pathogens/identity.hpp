#pragma once
#include <cstdint>
#include <unordered_map>

enum class PathogenTypes : uint8_t {
    VIRUS
};

struct PathogenId {
    uint64_t value;
    PathogenTypes type;
};

template <>
struct std::hash<PathogenId> {
    std::size_t operator()(const PathogenId& id) const noexcept {
        std::size_t h1 = std::hash<uint64_t>{}(id.value);
        std::size_t h2 = std::hash<PathogenTypes>{}(id.type);

        return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
    }
};


