#pragma once
#include "karkinolution/terrain/soil.hpp"
#include <cstddef>
#include <cstdint>
#include <karkinolution/terrain/rtree/box.hpp>
#include <karkinolution/terrain/rtree/node.hpp>
#include <optional>

struct FindSplitOutput {
  size_t best_index;
  double total_score;
};

struct GetBestOrdenationOutput {
  size_t best_index;
  std::vector<Entry *> entries;
};
struct SplitOutput {
  std::vector<Entry *> left, right;
};

struct FindSoilPieceOutput {
  std::vector<Node *> path;
  std::optional<size_t> index = std::nullopt;
  bool found;
};

struct _Internal_FindSoilPieceOutput {
  bool found;
  std::optional<size_t> index = std::nullopt;
};

struct OrphanEntry {
  Entry entry;
  size_t depth;
};

namespace RStarTreeMotor {
Box3D calculate_mbr(const std::vector<Entry> &, size_t begin, size_t end);
Box3D calculate_mbr(const std::vector<Entry *> &, size_t begin, size_t end);
GetBestOrdenationOutput get_best_ordenation(std::vector<Entry> &,
                                            size_t min_entries);
FindSplitOutput find_best_split_intersection(std::vector<Entry *> &,
                                             size_t min_entries);
FindSplitOutput find_best_split_intersection(std::vector<Entry> &,
                                             size_t min_entries);
FindSplitOutput find_best_split_margin(std::vector<Entry *> &,
                                       size_t min_entries);
SplitOutput split(std::vector<Entry> &entries, size_t min_entries);
} // namespace RStarTreeMotor

class RStarTree {
public:
  static constexpr size_t MAX_ENTRIES = 8;
  static constexpr size_t MIN_ENTRIES = 3;
  static constexpr size_t MIN_P_ENLARGEMENT_CANDITATES = 32;

  RStarTree();

  void insert_entry(Entry entry, std::vector<Node *> path,
                    bool already_reinserted);
  void insert(SoilPieceId id, const Box3D &box, bool already_reinserted);
  void reinsert_orphan(Entry entry, size_t depth);
  void split(Node &leaf, Node *parent);
  void split_root(Node &old_root);
  size_t choose_by_enlargement(Node &node, const Box3D &box);
  size_t choose_by_overlap(Node &node, const Box3D &box);

  void choose_leaf(std::vector<Node *> &nodes, const Box3D &box, size_t depth);

  std::vector<SoilPieceId> find(const Box3D &box) const;

  FindSoilPieceOutput find_soil_piece(SoilPieceId id);

  bool delete_soil_piece(SoilPieceId id);

private:
  std::unique_ptr<Node> root_;

  static void expand(Node &node, const Box3D &box);
  std::vector<SoilPieceId> find(const Box3D &box,
                                const std::vector<Entry> &entries,
                                std::vector<SoilPieceId> &ids) const;

  _Internal_FindSoilPieceOutput find_soil_piece(SoilPieceId id,
                                                std::vector<Node *> &path);
};