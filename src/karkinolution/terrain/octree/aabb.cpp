#include <cstddef>
#include <karkinolution/terrain/octree/aabb.hpp>
#include <optional>

void OctreeNode::subdivide() {
    const auto center = bounds_.center();

    for (int i = 0; i < 8; ++i) {
        Vec3 child_min;
        Vec3 child_max;

        child_min.x = (i & 1) ? center.x : bounds_.min.x;
        child_max.x = (i & 1) ? bounds_.max.x : center.x;

        child_min.y = (i & 2) ? center.y : bounds_.min.y;
        child_max.y = (i & 2) ? bounds_.max.y : center.y;

        child_min.z = (i & 4) ? center.z : bounds_.min.z;
        child_max.z = (i & 4) ? bounds_.max.z : center.z;

        children[i] = std::make_unique<OctreeNode>(AABB{child_min, child_max});
    }
}

std::optional<int> OctreeNode::child_containing(const AABB& other_bounds) const {
    if (is_leaf()) {
        return std::nullopt;
    }

    for (size_t i = 0; i < MAX_OCTREE_CHILDREN; i++) {
        if (children[i].get()->bounds_.contains(other_bounds)) {
            return static_cast<int>(i);
        }
    }

    return std::nullopt;
}

OctreeNode& OctreeNode::insert(const Entry& entry) {
    if (is_leaf()) {
        if (is_full()) {
            subdivide();

            for (size_t i = entries.size(); i-- > 0;) {
                auto& self_entry = entries[i];
                const auto index = child_containing(self_entry.bound);

                if (index.has_value()) {
                    children[index.value()]->insert(self_entry);
                    entries.erase(entries.begin() + i);
                }
            }

            const auto index = child_containing(entry.bound);

            if (index.has_value()) {
                children[index.value()]->insert(entry);
                return *children[index.value()];
            }

            entries.push_back(entry);
            return *this;
        } else {
            entries.push_back(entry);
            return *this;
        }
    } else {
        const auto index = child_containing(entry.bound);

        if (index.has_value()) {
            children[index.value()]->insert(entry);
            return *children[index.value()];
        }

        entries.push_back(entry);
        return *this;
    }
}

bool OctreeNode::remove(Id id) {
    for (size_t i = entries.size(); i-- > 0;) {
        if (entries[i].entity_id == id) {
            entries.erase(entries.begin() + i);
            return true;
        }
    }

    if (is_leaf()) {
        return false;
    }

    for (auto& child : children) {
        if (child == nullptr) {
            continue;
        }

        const auto had_remove = child->remove(id);

        if (had_remove) {
            return true;
        }
    }

    return false;
}

bool OctreeNode::remove(Id id, const AABB& old_box) {
    for (size_t i = entries.size(); i-- > 0;) {
        if (entries[i].entity_id == id) {
            entries.erase(entries.begin() + i);
            return true;
        }
    }

    if (is_leaf()) {
        return false;
    }

    const auto index = child_containing(old_box);

    if (!index.has_value()) {
        return false;
    }

    return children[index.value()]->remove(id, old_box);
}

bool OctreeNode::update(Id id, const AABB& new_box) {
    const auto had_remove = remove(id);

    if (!had_remove) {
        return false; // if removal fails, insertion does not execute
    }

    insert(Entry{.entity_id = id, .bound = new_box});
    return true;
}

bool OctreeNode::update(Id id, const AABB& old_box, const AABB& new_box) {
    const auto had_remove = remove(id, old_box);

    if (!had_remove) {
        return false; // if removal fails, insertion does not execute
    }

    insert(Entry{.entity_id = id, .bound = new_box});
    return true;
}

bool OctreeNode::exists(Id id) const {
    for (size_t i = entries.size(); i-- > 0;) {
        if (entries[i].entity_id == id) {
            return true;
        }
    }

    if (is_leaf()) {
        return false;
    }

    for (const auto& child : children) {
        if (child == nullptr) {
            continue;
        }

        const auto found = child->exists(id);

        if (found) {
            return true;
        }
    }

    return false;
}

bool OctreeNode::exists(Id id, const AABB& aabb) const {
    for (size_t i = entries.size(); i-- > 0;) {
        if (entries[i].entity_id == id) {
            return true;
        }
    }

    if (is_leaf()) {
        return false;
    }

    const auto index = child_containing(aabb);

    if (!index.has_value()) {
        return false;
    }

    return children[index.value()]->exists(id, aabb);
}

void OctreeNode::find(const AABB& aabb, std::vector<Entry*>& output_entries) {
    if (!bounds_.intersects(aabb)) {
        return;
    }

    for (auto& entry : entries) {
        if (entry.bound.intersects(aabb)) {
            output_entries.push_back(&entry);
        }
    }

    if (is_leaf()) {
        return;
    }

    for (auto& child : children) {
        if (child == nullptr) {
            continue;
        }

        child->find(aabb, output_entries);
    }
}

void OctreeNode::find(const AABB& aabb, std::vector<const Entry*>& output_entries) const {
    if (!bounds_.intersects(aabb)) {
        return;
    }

    for (const auto& entry : entries) {
        if (entry.bound.intersects(aabb)) {
            output_entries.push_back(&entry);
        }
    }

    if (is_leaf()) {
        return;
    }

    for (const auto& child : children) {
        if (child == nullptr) {
            continue;
        }

        child->find(aabb, output_entries);
    }
}

std::vector<Entry*> OctreeNode::find(const AABB& aabb) {
    std::vector<Entry*> output{};
    find(aabb, output);
    return output;
}

std::vector<const Entry*> OctreeNode::find(const AABB& aabb) const {
    std::vector<const Entry*> output{};
    find(aabb, output);
    return output;
}