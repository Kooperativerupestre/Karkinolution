#pragma once
#include <array>
#include <cstddef>
#include <karkinolution/math/physic/vec/model.hpp>
#include <karkinolution/organism/entities/identity.hpp>
#include <memory>
#include <vector>

inline constexpr size_t MAX_OCTREE_CHILDREN = 8;
inline constexpr size_t MAX_ENTRIES = 8;

struct AABB {
    Vec3 max;
    Vec3 min;

    [[nodiscard]] Vec3 center() const noexcept { return (min + max) / 2.0; }
    [[nodiscard]] bool contains(const AABB& other) const {
        return min.x <= other.min.x && min.y <= other.min.y && min.z <= other.min.z &&
               max.x >= other.max.x && max.y >= other.max.y && max.z >= other.max.z;
    }
    [[nodiscard]] bool intersects(const AABB& other) const {
        return min.x <= other.max.x && max.x >= other.min.x && min.y <= other.max.y &&
               max.y >= other.min.y && min.z <= other.max.z && max.z >= other.min.z;
    }
    [[nodiscard]] double volume() const noexcept {
        const auto size = max - min;
        return size.x * size.y * size.z;
    }
};

struct Entry {
    Id entity_id;
    AABB bound;
};

class Node {
  private:
    AABB bounds_;
    std::vector<Entry> entries{};
    std::array<std::unique_ptr<Node>, MAX_OCTREE_CHILDREN> children{};

    void find(const AABB& aabb, std::vector<Entry*>& output_entries);
    void find(const AABB& aabb, std::vector<const Entry*>& output_entries) const;

  public:
    Node(const AABB& aabb) : bounds_(aabb) {}

    size_t children_size() const {
        size_t count = 0;
        for (const auto& node : children) {
            if (node != nullptr) {
                count++;
            }
        }
        return count;
    }

    const AABB& bounds() const { return bounds_; }
    const std::vector<Entry>& entries_view() const { return entries; };
    const std::array<std::unique_ptr<Node>, MAX_OCTREE_CHILDREN>& children_view() const {
        return children;
    };

    bool is_leaf() const { return children[0] == nullptr; }
    bool is_full() const { return entries.size() == MAX_ENTRIES; }
    void subdivide();
    std::optional<int> child_containing(const AABB& bounds) const;
    Node& insert(const Entry& entry);
    bool remove(Id id);

    bool remove(Id id, const AABB& old_box);

    bool update(Id id, const AABB& old_box, const AABB& new_box);
    bool update(Id id, const AABB& new_box);

    // Update is logically atomic

    bool exists(Id id) const;
    bool exists(Id id, const AABB& old_box) const;

    std::vector<Entry*> find(const AABB& aabb);
    std::vector<const Entry*> find(const AABB& aabb) const;

    /*
func(...) -> Do if it exists
func(..., old_box) -> Do if it exists and the correct old_box is provided



Efficiency:

func(..., old_box) is more efficient than func(...)
*/
};
