#include "karkinolution/terrain/rtree/box.hpp"
#include "karkinolution/terrain/rtree/node.hpp"
#include "karkinolution/terrain/soil.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <karkinolution/math/physic/vec/model.hpp>
#include <karkinolution/terrain/rtree/rtree.hpp>
#include <limits>
#include <memory>
#include <numeric>
#include <optional>
#include <pthread.h>
#include <sys/types.h>
#include <utility>
#include <variant>

RStarTree::RStarTree()
    : root_(std::make_unique<Node>(
          Node{.type = NodeType::LEAF, .box = std::nullopt, .entries = {}})) {}

Box3D RStarTreeMotor::calculate_mbr(const std::vector<Entry> &entries,
                                    size_t begin, size_t end) {
  Box3D mbr = entries[begin].box;

  for (size_t i = begin + 1; i < end; i++) {
    mbr = Box3DMotor::combine(mbr, entries[i].box);
  }
  return mbr;
}

Box3D RStarTreeMotor::calculate_mbr(const std::vector<Entry *> &entries,
                                    size_t begin, size_t end) {
  Box3D mbr = entries[begin]->box;

  for (size_t i = begin + 1; i < end; i++) {
    mbr = Box3DMotor::combine(mbr, entries[i]->box);
  }
  return mbr;
}

GetBestOrdenationOutput
RStarTreeMotor::get_best_ordenation(std::vector<Entry> &entries,
                                    size_t min_entries) {
  std::array<std::array<std::vector<Entry *>, 2>, 3> ordenations;

  for (auto &entry : entries) {
    ordenations[index(Axis::X)][index(Bound::MAX)].push_back(&entry);
    ordenations[index(Axis::X)][index(Bound::MIN)].push_back(&entry);
    ordenations[index(Axis::Y)][index(Bound::MAX)].push_back(&entry);
    ordenations[index(Axis::Y)][index(Bound::MIN)].push_back(&entry);
    ordenations[index(Axis::Z)][index(Bound::MAX)].push_back(&entry);
    ordenations[index(Axis::Z)][index(Bound::MIN)].push_back(&entry);
  }

  std::ranges::sort(ordenations[index(Axis::X)][index(Bound::MAX)],
                    [](const Entry *A, const Entry *B) {
                      return std::pair{A->box.max.x, A->box.min.x} <
                             std::pair{B->box.max.x, B->box.min.x};
                    });

  std::ranges::sort(ordenations[index(Axis::X)][index(Bound::MIN)],
                    [](const Entry *A, const Entry *B) {
                      return std::pair{A->box.min.x, A->box.max.x} <
                             std::pair{B->box.min.x, B->box.max.x};
                    });

  std::ranges::sort(ordenations[index(Axis::Y)][index(Bound::MAX)],
                    [](const Entry *A, const Entry *B) {
                      return std::pair{A->box.max.y, A->box.min.y} <
                             std::pair{B->box.max.y, B->box.min.y};
                    });

  std::ranges::sort(ordenations[index(Axis::Y)][index(Bound::MIN)],
                    [](const Entry *A, const Entry *B) {
                      return std::pair{A->box.min.y, A->box.max.y} <
                             std::pair{B->box.min.y, B->box.max.y};
                    });

  std::ranges::sort(ordenations[index(Axis::Z)][index(Bound::MAX)],
                    [](const Entry *A, const Entry *B) {
                      return std::pair{A->box.max.z, A->box.min.z} <
                             std::pair{B->box.max.z, B->box.min.z};
                    });

  std::ranges::sort(ordenations[index(Axis::Z)][index(Bound::MIN)],
                    [](const Entry *A, const Entry *B) {
                      return std::pair{A->box.min.z, A->box.max.z} <
                             std::pair{B->box.min.z, B->box.max.z};
                    });

  double best_score = std::numeric_limits<double>::infinity();
  std::vector<Entry *> *ptr = nullptr;
  for (auto &axis : ordenations) {
    for (auto &bound : axis) {
      auto output = find_best_split_margin(bound, min_entries);
      double total_score = output.total_score;
      if (total_score < best_score) {
        best_score = total_score;
        ptr = &bound;
      }
    }
  }
  return {.best_index =
              find_best_split_intersection(*ptr, min_entries).best_index,
          .entries = *ptr};
}

FindSplitOutput
RStarTreeMotor::find_best_split_margin(std::vector<Entry *> &entries,
                                       size_t min_entries) {
  size_t size = entries.size();
  std::vector<Box3D> prefix;
  std::vector<Box3D> suffix;
  prefix.reserve(size);
  suffix.reserve(size);

  prefix.resize(size);
  suffix.resize(size);

  prefix[0] = entries[0]->box;

  suffix[size - 1] = entries[size - 1]->box;
  for (size_t i = size - 1; i > 0; i--) {
    suffix[i - 1] = Box3DMotor::combine(suffix[i], entries[i - 1]->box);
  }
  for (size_t i = 1; i < size; i++) {
    prefix[i] = Box3DMotor::combine(prefix[i - 1], entries[i]->box);
  }

  size_t best_index = min_entries;

  double best_score =
      prefix[best_index - 1].margin() + suffix[best_index].margin();
  double total_score = best_score;
  double best_volume_sum =
      prefix[best_index - 1].volume() + suffix[best_index].volume();
  for (size_t i = min_entries + 1; i <= size - min_entries; i++) {
    const double current_score = prefix[i - 1].margin() + suffix[i].margin();
    const double current_volume = prefix[i - 1].volume() + suffix[i].volume();
    total_score += current_score;

    if (current_score < best_score ||
        current_score == best_score && current_volume < best_volume_sum) {
      best_score = current_score;
      best_index = i;
      best_volume_sum = current_volume;
    }
  }
  return {.best_index = best_index, .total_score = total_score};
}

FindSplitOutput
RStarTreeMotor::find_best_split_intersection(std::vector<Entry *> &entries,
                                             size_t min_entries) {
  size_t size = entries.size();
  std::vector<Box3D> prefix;
  std::vector<Box3D> suffix;
  prefix.reserve(size);
  suffix.reserve(size);
  prefix.resize(size);
  suffix.resize(size);

  prefix[0] = entries[0]->box;
  suffix[size - 1] = entries[size - 1]->box;
  for (size_t i = size - 1; i > 0; i--) {
    suffix[i - 1] = Box3DMotor::combine(suffix[i], entries[i - 1]->box);
  }
  for (size_t i = 1; i < size; i++) {
    prefix[i] = Box3DMotor::combine(prefix[i - 1], entries[i]->box);
  }

  size_t best_index = min_entries;

  double best_score =
      Box3DMotor::overlap_volume(prefix[best_index - 1], suffix[best_index]);
  double total_score = best_score;
  double best_volume_sum =
      prefix[best_index - 1].volume() + suffix[best_index].volume();
  double current_score, current_volume;
  for (size_t i = min_entries + 1; i <= size - min_entries; i++) {
    current_score = Box3DMotor::overlap_volume(prefix[i - 1], suffix[i]);
    current_volume = prefix[i - 1].volume() + suffix[i].volume();

    total_score += current_score;
    if (current_score < best_score ||
        current_score == best_score && current_volume < best_volume_sum) {
      best_score = current_score;
      best_index = i;
      best_volume_sum = current_volume;
    }
  }
  return {.best_index = best_index, .total_score = total_score};
}

size_t RStarTree::choose_by_enlargement(Node &node, const Box3D &box) {
  size_t best_index = 0;
  double best_enlargement = Box3DMotor::enlargement(node.entries[0].box, box);
  double best_volume = node.entries[0].box.volume();

  for (size_t i = 1; i < node.entries.size(); i++) {
    const Box3D &entry_box = node.entries[i].box;
    const double current_enlargement = Box3DMotor::enlargement(entry_box, box);
    const double current_volume = entry_box.volume();

    if (current_enlargement < best_enlargement ||
        (current_enlargement == best_enlargement &&
         current_volume < best_volume)) {
      best_enlargement = current_enlargement;
      best_volume = current_volume;
      best_index = i;
    }
  }
  return best_index;
}

size_t RStarTree::choose_by_overlap(Node &node, const Box3D &box) {
  auto &node_children = node.entries;
  auto node_children_size = node_children.size();

  std::vector<Entry *> entries;
  entries.reserve(node_children_size);

  std::vector<double> enlargement_list;
  enlargement_list.reserve(node_children_size);

  for (auto &entry : node_children) {
    entries.push_back(&entry);
    enlargement_list.push_back(Box3DMotor::enlargement(entry.box, box));
  }

  std::vector<size_t> indices(entries.size());

  std::iota(indices.begin(), indices.end(), 0);

  std::sort(indices.begin(), indices.end(), [&](size_t a, size_t b) {
    return enlargement_list[a] < enlargement_list[b];
  });

  size_t best_index = 0;
  double best_overlap = std::numeric_limits<double>().max();

  for (size_t k = 0; k < MIN_P_ENLARGEMENT_CANDITATES && k < indices.size();
       k++) {
    size_t j = indices[k];
    const auto box_expanded = Box3DMotor::combine(node_children[j].box, box);
    double current_overlap = 0;
    double new_overlap = 0;
    for (size_t i = 0; i < node_children_size; i++) {
      if (i == j) {
        continue;
      } else {
        current_overlap +=
            Box3DMotor::overlap_volume(entries[j]->box, node_children[i].box);
        new_overlap +=
            Box3DMotor::overlap_volume(box_expanded, node_children[i].box);
      }
    }
    const double delta_overlap = new_overlap - current_overlap;
    if (delta_overlap < best_overlap) {
      best_index = j;
      best_overlap = delta_overlap;
    }
  }
  return best_index;
}

void RStarTree::choose_leaf(std::vector<Node *> &nodes, const Box3D &box) {
  auto &last_node = nodes.back();

  if (last_node->type == NodeType::LEAF) {
    return;
  }

  bool children_are_leaves = false;
  if (std::holds_alternative<std::unique_ptr<Node>>(
          last_node->entries[0].content)) {
    auto &first_child =
        *std::get<std::unique_ptr<Node>>(last_node->entries[0].content);
    children_are_leaves = (first_child.type == NodeType::LEAF);
  }

  size_t best_index = children_are_leaves
                          ? choose_by_overlap(*last_node, box)
                          : choose_by_enlargement(*last_node, box);

  Node &child =
      *std::get<std::unique_ptr<Node>>(last_node->entries[best_index].content);
  nodes.push_back(&child);
  choose_leaf(nodes, box);
}

SplitOutput RStarTreeMotor::split(std::vector<Entry> &entries,
                                  size_t min_entries) {
  auto best_ordenation = get_best_ordenation(entries, min_entries).entries;
  size_t split_index =
      find_best_split_intersection(best_ordenation, min_entries).best_index;

  std::vector<Entry *> right;
  std::vector<Entry *> left;

  for (size_t i = 0; i < best_ordenation.size(); i++) {
    if (i < split_index) {
      left.push_back(&*best_ordenation[i]);
    } else {
      right.push_back(&*best_ordenation[i]);
    }
  }
  return {.left = left, .right = right};
}

void RStarTree::split(Node &leaf, Node *parent) {
  const NodeType node_type = leaf.type;
  auto split_output = RStarTreeMotor::split(leaf.entries, MIN_ENTRIES);

  std::vector<Entry> left_entries;
  std::vector<Entry> right_entries;

  left_entries.reserve(split_output.left.size());
  right_entries.reserve(split_output.right.size());

  for (auto &entry : split_output.left) {
    left_entries.push_back(std::move(*entry));
  }

  for (auto &entry : split_output.right) {
    right_entries.push_back(std::move(*entry));
  }

  Node left_node{.type = node_type,
                 .box = RStarTreeMotor::calculate_mbr(left_entries, 0,
                                                      left_entries.size()),
                 .entries = std::move(left_entries)};
  Node right_node{.type = node_type,
                  .box = RStarTreeMotor::calculate_mbr(right_entries, 0,
                                                       right_entries.size()),
                  .entries = std::move(right_entries)};

  for (auto it = parent->entries.begin(); it != parent->entries.end(); it++) {
    if (std::holds_alternative<std::unique_ptr<Node>>(it->content) &&
        std::get<std::unique_ptr<Node>>(it->content).get() == &leaf) {
      parent->entries.erase(it);
      break;
    }
  };
  parent->entries.push_back(
      Entry{.box = left_node.box.value(),
            .content = std::make_unique<Node>(std::move(left_node))});
  parent->entries.push_back(
      Entry{.box = right_node.box.value(),
            .content = std::make_unique<Node>(std::move(right_node))});
}

void RStarTree::split_root(Node &old_root) {
  const NodeType node_type = old_root.type;
  auto split_output = RStarTreeMotor::split(old_root.entries, MIN_ENTRIES);

  std::vector<Entry> left_entries;
  std::vector<Entry> right_entries;
  left_entries.reserve(split_output.left.size());
  right_entries.reserve(split_output.right.size());

  for (auto *entry : split_output.left) {
    left_entries.push_back(std::move(*const_cast<Entry *>(entry)));
  }
  for (auto *entry : split_output.right) {
    right_entries.push_back(std::move(*const_cast<Entry *>(entry)));
  }

  Node left_node{.type = node_type,
                 .box = RStarTreeMotor::calculate_mbr(left_entries, 0,
                                                      left_entries.size()),
                 .entries = std::move(left_entries)};
  Node right_node{.type = node_type,
                  .box = RStarTreeMotor::calculate_mbr(right_entries, 0,
                                                       right_entries.size()),
                  .entries = std::move(right_entries)};

  auto new_root = std::make_unique<Node>();
  new_root->type = NodeType::INTERNAL;
  new_root->entries.push_back(
      Entry{.box = left_node.box.value(),
            .content = std::make_unique<Node>(std::move(left_node))});
  new_root->entries.push_back(
      Entry{.box = right_node.box.value(),
            .content = std::make_unique<Node>(std::move(right_node))});
  new_root->box = RStarTreeMotor::calculate_mbr(new_root->entries, 0,
                                                new_root->entries.size());

  root_ = std::move(new_root);
}

void RStarTree::insert_entry(Entry entry, std::vector<Node *> path,
                             bool already_reinserted) {
  auto &leaf = *path.back();
  size_t path_size = path.size();

  std::vector<Entry> removed_entries;

  leaf.entries.push_back(std::move(entry));
  size_t size = leaf.entries.size();

  if (size > MAX_ENTRIES) {
    if (already_reinserted) {
      if (path_size > 1) {
        auto &parent = path[path_size - 2];
        split(leaf, parent);
      } else {
        split_root(leaf);
      }
    } else {
      auto node_center =
          RStarTreeMotor::calculate_mbr(leaf.entries, 0, size).center();

      std::vector<size_t> ordened_indices(size);
      std::iota(ordened_indices.begin(), ordened_indices.end(), 0);

      std::ranges::sort(ordened_indices, [&](size_t a, size_t b) {
        return leaf.entries[a].box.center().distance_squared_to(node_center) >
               leaf.entries[b].box.center().distance_squared_to(node_center);
      });

      size_t count_to_remove = std::round(0.3f * MAX_ENTRIES);

      std::vector<size_t> indices_to_remove(
          ordened_indices.begin(), ordened_indices.begin() + count_to_remove);
      std::ranges::sort(indices_to_remove, std::greater<>());

      for (size_t idx : indices_to_remove) {
        removed_entries.push_back(std::move(leaf.entries[idx]));
        leaf.entries.erase(leaf.entries.begin() + idx);
      }

      leaf.box =
          RStarTreeMotor::calculate_mbr(leaf.entries, 0, leaf.entries.size());
    }
  }
  if (path.size() > 1) {
    for (size_t i = path.size() - 2; i > 0; i--) {
      auto &node = *path[i];
      if (node.entries.size() > MAX_ENTRIES) {
        auto &parent = path[i - 1];
        split(node, parent);
      }
    }
  }

  for (auto &e : removed_entries) {
    insert(std::get<SoilPieceId>(e.content), e.box, true);
  }

  if (root_->entries.size() > MAX_ENTRIES) {
    split_root(*root_);
  }
  refresh_mbrs(*root_);
}
void RStarTree::insert(SoilPieceId id, const Box3D &box,
                       bool already_reinserted) {
  std::vector<Node *> path{root_.get()};
  choose_leaf(path, box);
  insert_entry(Entry{.box = box, .content = id}, std::move(path),
               already_reinserted);
}

void RStarTree::reinsert_orphan(Entry entry, size_t depth) {
  std::vector<Node *> path{root_.get()};
  choose_node_at_depth(path, entry.box, depth);
  insert_entry(std::move(entry), std::move(path), true);
}

void RStarTree::choose_node_at_depth(std::vector<Node *> &nodes,
                                     const Box3D &box, size_t depth) {
  while (nodes.size() < depth) {
    Node &node = *nodes.back();

    bool children_are_leaves = false;
    if (!node.entries.empty() &&
        std::holds_alternative<std::unique_ptr<Node>>(
            node.entries[0].content)) {
      const auto &first_child =
          *std::get<std::unique_ptr<Node>>(node.entries[0].content);
      children_are_leaves = (first_child.type == NodeType::LEAF);
    }

    size_t best_index = children_are_leaves
                            ? choose_by_overlap(node, box)
                            : choose_by_enlargement(node, box);
    nodes.push_back(
        std::get<std::unique_ptr<Node>>(node.entries[best_index].content)
            .get());
  }
}

void RStarTree::refresh_mbrs(Node &node) {
  if (node.entries.empty()) {
    node.box = std::nullopt;
    return;
  }

  for (auto &entry : node.entries) {
    if (std::holds_alternative<std::unique_ptr<Node>>(entry.content)) {
      auto &child = *std::get<std::unique_ptr<Node>>(entry.content);
      refresh_mbrs(child);
      if (child.box.has_value()) {
        entry.box = child.box.value();
      }
    }
  }

  node.box =
      RStarTreeMotor::calculate_mbr(node.entries, 0, node.entries.size());
}
std::vector<SoilPieceId> RStarTree::find(const Box3D &box) const {
  std::vector<SoilPieceId> ids;

  for (const auto &entry : root_->entries) {
    if (Box3DMotor::intersect(box, entry.box)) {
      if (std::holds_alternative<SoilPieceId>(entry.content)) {
        ids.push_back(std::get<SoilPieceId>(entry.content));
      } else {
        find(box, std::get<std::unique_ptr<Node>>(entry.content)->entries, ids);
      }
    }
  }
  return ids;
}

std::vector<SoilPieceId> RStarTree::find(const Box3D &box,
                                         const std::vector<Entry> &entries,
                                         std::vector<SoilPieceId> &ids) const {
  for (const auto &entry : entries) {
    if (Box3DMotor::intersect(box, entry.box)) {
      if (std::holds_alternative<SoilPieceId>(entry.content)) {
        ids.push_back(std::get<SoilPieceId>(entry.content));
      } else {
        find(box, std::get<std::unique_ptr<Node>>(entry.content)->entries, ids);
      }
    }
  }
  return ids;
}

FindSoilPieceOutput RStarTree::find_soil_piece(SoilPieceId id) {
  std::vector<Node *> path = {root_.get()};
  auto output = find_soil_piece(id, path);
  return {.path = path, .index = output.index, .found = output.found};
}

_Internal_FindSoilPieceOutput
RStarTree::find_soil_piece(SoilPieceId id, std::vector<Node *> &path) {
  for (size_t i = 0; i < path.back()->entries.size(); i++) {
    auto &entry = path.back()->entries[i];
    if (std::holds_alternative<SoilPieceId>(entry.content)) {
      if (std::get<SoilPieceId>(entry.content) == id) {
        return {.found = true, .index = i};
      }
    } else {
      auto &node = *std::get<std::unique_ptr<Node>>(entry.content);
      path.push_back(&node);
      auto output = find_soil_piece(id, path);

      if (output.found) {
        return {.found = true, .index = output.index};
      } else {
        path.erase(path.end() - 1);
      }
    }
  }
  return {.found = false};
}

bool RStarTree::delete_soil_piece(SoilPieceId id) {
  auto find_output = find_soil_piece(id);

  if (!find_output.found) {
    return false;
  }
  auto &path = find_output.path;
  auto &path_entries = path.back()->entries;
  auto &leaf = *path.back();
  auto &entry_of_removed = path.back()->entries[find_output.index.value()];
  std::vector<OrphanEntry> orphaned_entries;

  path_entries.erase(path_entries.begin() + find_output.index.value());

  for (size_t i = path.size() - 1; i > 0; i--) {
    auto &node = *path[i];
    auto &parent = *path[i - 1];
    const auto &node_size = node.entries.size();

    if (node_size < MIN_ENTRIES) {
      for (size_t n_i = node_size; n_i-- > 0;) {
        orphaned_entries.push_back(
            OrphanEntry{.entry = std::move(node.entries[n_i]), .depth = i});

        node.entries.erase(node.entries.begin() + n_i);
      }

      for (size_t p_i = parent.entries.size(); p_i-- > 0;) {
        if (std::holds_alternative<std::unique_ptr<Node>>(
                parent.entries[p_i].content) &&
            &*std::get<std::unique_ptr<Node>>(parent.entries[p_i].content) ==
                &node) {
          parent.entries.erase(parent.entries.begin() + p_i);
          break;
        }
      }
    } else {
      node.box =
          RStarTreeMotor::calculate_mbr(node.entries, 0, node.entries.size());
    }
  }

  for (auto &entry : orphaned_entries) {
    if (std::holds_alternative<SoilPieceId>(entry.entry.content)) {

      insert(std::get<SoilPieceId>(entry.entry.content), entry.entry.box,
             false);
    } else {
      reinsert_orphan(std::move(entry.entry), entry.depth);
    }
  }

  if (root_->entries.size() == 1 &&
      std::holds_alternative<std::unique_ptr<Node>>(
          root_->entries.back().content)) {
    root_ = std::move(
        std::get<std::unique_ptr<Node>>(root_->entries.back().content));
  }
  refresh_mbrs(*root_);
  return true;
}

bool RStarTree::exists(SoilPieceId id) const { return exists_impl(*root_, id); }

bool RStarTree::exists_impl(const Node &node, SoilPieceId id) const {
  for (const auto &entry : node.entries) {
    if (std::holds_alternative<SoilPieceId>(entry.content)) {
      if (std::get<SoilPieceId>(entry.content) == id) {
        return true;
      }
    } else {
      const auto &child = *std::get<std::unique_ptr<Node>>(entry.content);
      if (exists_impl(child, id)) {
        return true;
      }
    }
  }
  return false;
}
