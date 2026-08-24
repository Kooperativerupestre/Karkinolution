#pragma once
#include <karkinolution/terrain/soil.hpp>
#include <karkinolution/terrain/rtree/box.hpp>

enum class NodeType : uint8_t {
    LEAF,
    INTERNAL
};

struct Node;

using EntryContentType = std::variant<SoilPieceId, std::unique_ptr<Node>>;

struct Entry {
    Box3D box;
    EntryContentType content;
};

struct Node {
    NodeType type;
    std::optional<Box3D> box;
    std::vector<Entry> entries;
};

