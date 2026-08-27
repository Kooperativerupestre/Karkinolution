#pragma once
#include <karkinolution/terrain/rtree/box.hpp>
#include <karkinolution/terrain/soil.hpp>

enum class NodeType : uint8_t { LEAF, INTERNAL };

struct RtreeNode;

using EntryContentType = std::variant<SoilPieceId, std::unique_ptr<RtreeNode>>;

struct RtreeEntry {
    Box3D box;
    EntryContentType content;
};

struct RtreeNode {
    NodeType type;
    std::optional<Box3D> box;
    std::vector<RtreeEntry> entries;
};
