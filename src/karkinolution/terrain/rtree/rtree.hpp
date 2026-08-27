#pragma once
#include "karkinolution/terrain/soil.hpp"
#include <cstddef>
#include <karkinolution/terrain/rtree/box.hpp>
#include <karkinolution/terrain/rtree/node.hpp>
#include <optional>

struct FindSplitOutput {
    size_t best_index;
    double total_score;
};

struct GetBestOrdenationOutput {
    size_t best_index;
    std::vector<RtreeEntry*> entries;
};
struct SplitOutput {
    std::vector<RtreeEntry*> left, right;
};

struct FindSoilPieceOutput {
    std::vector<RtreeNode*> path;
    std::optional<size_t> index = std::nullopt;
    bool found;
};

struct _Internal_FindSoilPieceOutput {
    bool found;
    std::optional<size_t> index = std::nullopt;
};

struct OrphanEntry {
    RtreeEntry entry;
    size_t depth;
};

namespace RStarTreeMotor {
Box3D calculate_mbr(const std::vector<RtreeEntry>&, size_t begin, size_t end);
Box3D calculate_mbr(const std::vector<RtreeEntry*>&, size_t begin, size_t end);
GetBestOrdenationOutput get_best_ordenation(std::vector<RtreeEntry>&, size_t min_entries);
FindSplitOutput find_best_split_intersection(std::vector<RtreeEntry*>&, size_t min_entries);
FindSplitOutput find_best_split_intersection(std::vector<RtreeEntry>&, size_t min_entries);
FindSplitOutput find_best_split_margin(std::vector<RtreeEntry*>&, size_t min_entries);
SplitOutput split(std::vector<RtreeEntry>& entries, size_t min_entries);
} // namespace RStarTreeMotor

class RStarTree {
  public:
    static constexpr size_t MAX_ENTRIES = 8;
    static constexpr size_t MIN_ENTRIES = 3;
    static constexpr size_t MIN_P_ENLARGEMENT_CANDITATES = 32;

    RStarTree();

    void insert(SoilPieceId id, const Box3D& box, bool already_reinserted = false);

    std::vector<SoilPieceId> find(const Box3D& box) const;

    bool delete_soil_piece(SoilPieceId id);
    bool exists(SoilPieceId id) const;

    const RtreeNode& root() const { return *root_.get(); }

  private:
    std::unique_ptr<RtreeNode> root_;

    void split(RtreeNode& leaf, RtreeNode* parent);
    void split_root(RtreeNode& old_root);

    std::vector<SoilPieceId> find(const Box3D& box, const std::vector<RtreeEntry>& entries,
                                  std::vector<SoilPieceId>& ids) const;

    _Internal_FindSoilPieceOutput find_soil_piece(SoilPieceId id, std::vector<RtreeNode*>& path);

    void insert_entry(RtreeEntry entry, std::vector<RtreeNode*> path, bool already_reinserted);

    size_t choose_by_enlargement(RtreeNode& node, const Box3D& box);
    size_t choose_by_overlap(RtreeNode& node, const Box3D& box);

    void choose_leaf(std::vector<RtreeNode*>& nodes, const Box3D& box);
    void reinsert_orphan(RtreeEntry entry, size_t depth);
    void choose_node_at_depth(std::vector<RtreeNode*>& nodes, const Box3D& box, size_t depth);
    void refresh_mbrs(RtreeNode& node);
    FindSoilPieceOutput find_soil_piece(SoilPieceId id);
    bool exists_impl(const RtreeNode& node, SoilPieceId id) const;
};
