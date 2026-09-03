#pragma once
#include <karkinolution/terrain/rtree/box.hpp>
#include <karkinolution/terrain/soil.hpp>

enum class NodeType : uint8_t {
	LEAF,
	INTERNAL
};

template <typename IdType> struct RtreeNode;

template <typename IdType> struct RtreeEntry {
		Box3D                                                    box;
		std::variant<IdType, std::unique_ptr<RtreeNode<IdType>>> content;
};

template <typename IdType> struct RtreeNode {
		NodeType                        type;
		std::optional<Box3D>            box;
		std::vector<RtreeEntry<IdType>> entries;
};
