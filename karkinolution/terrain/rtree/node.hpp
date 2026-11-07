#pragma once
#include <karkinolution/terrain/rtree/box.hpp>
#include <karkinolution/terrain/soil/soil.hpp>

namespace Karkinolution::Terrain::Trees::RTree {
	enum class NodeType : uint8_t {
		LEAF,
		INTERNAL
	};

	template <typename IdType> struct RtreeNode;

	template <typename IdType> struct RtreeEntry {
			Box2D                                                    box;
			std::variant<IdType, std::unique_ptr<RtreeNode<IdType>>> content;
	};

	template <typename IdType> struct RtreeNode {
			NodeType                        type;
			std::optional<Box2D>            box;
			std::vector<RtreeEntry<IdType>> entries;
	};
} // namespace Karkinolution::Terrain::Trees::RTree
