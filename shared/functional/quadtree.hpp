#pragma once

#include "shared/math/physic/vec/model.hpp"

#include <array>
#include <cstddef>
#include <memory>
#include <optional>
#include <unordered_set>
#include <utility>
#include <vector>

namespace KShared::Functional::Quadtree {

	using Position = Math::Vec::Vec2;

	struct AABB {
			Position min;
			Position max;

			[[nodiscard]] constexpr Position center() const noexcept {
				return (min + max) / 2.0;
			}

			[[nodiscard]] constexpr bool contains(const Position &point) const noexcept {
				return point.x >= min.x && point.x <= max.x && point.y >= min.y && point.y <= max.y;
			}

			[[nodiscard]] constexpr bool contains(const AABB &other) const noexcept {
				return contains(other.min) && contains(other.max);
			}

			[[nodiscard]] constexpr bool intersects(const AABB &other) const noexcept {
				return min.x <= other.max.x && max.x >= other.min.x && min.y <= other.max.y && max.y >= other.min.y;
			}

			[[nodiscard]] constexpr double area() const noexcept {
				const Position size = max - min;
				return size.x * size.y;
			}
	};

	enum class Quadrant : unsigned char {
		UPPER_LEFT,
		UPPER_RIGHT,
		LOWER_LEFT,
		LOWER_RIGHT
	};

	inline constexpr std::size_t             QUADTREE_CHILDREN_THRESHOLD = 8;
	inline constexpr std::size_t             MAX_ENTRIES                 = 8;
	inline constexpr std::size_t             MAX_DEPTH                   = 8;
	inline constexpr std::array<Quadrant, 4> ALL_QUADRANTS{Quadrant::LOWER_LEFT,
														   Quadrant::LOWER_RIGHT,
														   Quadrant::UPPER_LEFT,
														   Quadrant::UPPER_RIGHT};

	enum class UpdateOutput : unsigned char {
		DOES_NOT_EXIST,
		UPDATED,
		EXISTS_BUT_WAS_REMOVED
	};

	template <typename IdType> struct Entry {
			IdType component_id;
			AABB   bounds;
	};

	template <typename IdType> class Node {
		private:

			AABB                       boundary;
			std::unique_ptr<Node>      upper_left;
			std::unique_ptr<Node>      upper_right;
			std::unique_ptr<Node>      lower_left;
			std::unique_ptr<Node>      lower_right;
			std::vector<Entry<IdType>> entries;
			std::size_t                depth;

			std::unique_ptr<Node> &child(Quadrant quadrant) {
				switch (quadrant) {
					case Quadrant::UPPER_LEFT:
						return upper_left;
					case Quadrant::UPPER_RIGHT:
						return upper_right;
					case Quadrant::LOWER_LEFT:
						return lower_left;
					case Quadrant::LOWER_RIGHT:
						return lower_right;
				}
				std::unreachable();
			}

			const std::unique_ptr<Node> &child(Quadrant quadrant) const {
				return const_cast<Node*>(this)->child(quadrant);
			}

			[[nodiscard]] AABB quadrant_boundary(Quadrant quadrant) const {
				const Position middle = boundary.center();
				switch (quadrant) {
					case Quadrant::UPPER_LEFT:
						return {boundary.min, middle};
					case Quadrant::UPPER_RIGHT:
						return {{middle.x, boundary.min.y}, {boundary.max.x, middle.y}};
					case Quadrant::LOWER_LEFT:
						return {{boundary.min.x, middle.y}, {middle.x, boundary.max.y}};
					case Quadrant::LOWER_RIGHT:
						return {middle, boundary.max};
				}
				std::unreachable();
			}

			[[nodiscard]] std::optional<Quadrant> containing_quadrant(const AABB &bound) const {
				for (const auto quadrant : ALL_QUADRANTS) {
					if (quadrant_boundary(quadrant).contains(bound)) {
						return quadrant;
					}
				}
				return std::nullopt;
			}

			bool remove_impl(const IdType &id, const AABB* old_bound) {
				if (old_bound != nullptr && !boundary.contains(*old_bound)) {
					return false;
				}
				for (std::size_t index = entries.size(); index-- > 0;) {
					if (entries[index].component_id == id) {
						entries.erase(entries.begin() + static_cast<std::ptrdiff_t>(index));
						return true;
					}
				}
				if (is_leaf()) {
					return false;
				}
				if (old_bound != nullptr) {
					const auto quadrant = containing_quadrant(*old_bound);
					return quadrant.has_value() && child(*quadrant)->remove_impl(id, old_bound);
				}
				for (const auto quadrant : ALL_QUADRANTS) {
					if (child(quadrant)->remove_impl(id, nullptr)) {
						return true;
					}
				}
				return false;
			}

			void find_impl(const AABB &area, std::vector<const Entry<IdType>*> &result) const {
				if (!boundary.intersects(area)) {
					return;
				}
				for (const auto &entry : entries) {
					if (entry.bounds.intersects(area)) {
						result.push_back(&entry);
					}
				}
				if (!is_leaf()) {
					for (const auto quadrant : ALL_QUADRANTS) {
						child(quadrant)->find_impl(area, result);
					}
				}
			}

		public:

			explicit Node(const AABB &bounds, std::size_t depth = 0)
				: boundary(bounds)
				, depth(depth) {}

			~Node()                           = default;
			Node(const Node &)                = delete;
			Node &operator=(const Node &)     = delete;
			Node(Node &&) noexcept            = default;
			Node &operator=(Node &&) noexcept = default;

			[[nodiscard]] bool is_leaf() const noexcept {
				return !upper_left && !upper_right && !lower_left && !lower_right;
			}

			[[nodiscard]] bool is_full() const noexcept {
				return entries.size() >= QUADTREE_CHILDREN_THRESHOLD;
			}

			[[nodiscard]] bool can_contain(const AABB &bound) const noexcept {
				return boundary.contains(bound);
			}

			[[nodiscard]] bool is_at_deepest_height() const noexcept {
				return depth >= MAX_DEPTH;
			}

			[[nodiscard]] const AABB &bounds() const noexcept {
				return boundary;
			}

			[[nodiscard]] const std::vector<Entry<IdType>> &entries_view() const noexcept {
				return entries;
			}

			[[nodiscard]] std::size_t children_size() const noexcept {
				return static_cast<std::size_t>(upper_left != nullptr)
					+ static_cast<std::size_t>(upper_right != nullptr) + static_cast<std::size_t>(lower_left != nullptr)
					+ static_cast<std::size_t>(lower_right != nullptr);
			}

			bool subdivide() {
				if (is_at_deepest_height()) {
					return false;
				}
				for (const auto quadrant : ALL_QUADRANTS) {
					child(quadrant) = std::make_unique<Node>(quadrant_boundary(quadrant), depth + 1);
				}
				return true;
			}

			bool insert(const Entry<IdType> &entry) {
				if (!can_contain(entry.bounds)) {
					return false;
				}
				if (is_leaf()) {
					if (!is_full() || !subdivide()) {
						entries.push_back(entry);
						return true;
					}
					for (std::size_t index = entries.size(); index-- > 0;) {
						const auto quadrant = containing_quadrant(entries[index].bounds);
						if (quadrant.has_value() && child(*quadrant)->insert(entries[index])) {
							entries.erase(entries.begin() + static_cast<std::ptrdiff_t>(index));
						}
					}
				}
				const auto quadrant = containing_quadrant(entry.bounds);
				if (quadrant.has_value() && child(*quadrant)->insert(entry)) {
					return true;
				}
				entries.push_back(entry);
				return true;
			}

			bool remove(const IdType &id) {
				return remove_impl(id, nullptr);
			}

			bool remove(const IdType &id, const AABB &old_bound) {
				return remove_impl(id, &old_bound);
			}

			UpdateOutput update(const IdType &id, const AABB &new_bound) {
				if (!can_contain(new_bound) || !remove(id)) {
					return UpdateOutput::DOES_NOT_EXIST;
				}
				return insert(Entry<IdType>{id, new_bound}) ? UpdateOutput::UPDATED
															: UpdateOutput::EXISTS_BUT_WAS_REMOVED;
			}

			UpdateOutput update(const IdType &id, const AABB &new_bound, const AABB &old_bound) {
				if (!can_contain(new_bound) || !remove(id, old_bound)) {
					return UpdateOutput::DOES_NOT_EXIST;
				}
				return insert(Entry<IdType>{id, new_bound}) ? UpdateOutput::UPDATED
															: UpdateOutput::EXISTS_BUT_WAS_REMOVED;
			}

			[[nodiscard]] bool exists(const IdType &id) const {
				for (const auto &entry : entries) {
					if (entry.component_id == id) {
						return true;
					}
				}
				if (!is_leaf()) {
					for (const auto quadrant : ALL_QUADRANTS) {
						if (child(quadrant)->exists(id)) {
							return true;
						}
					}
				}
				return false;
			}

			[[nodiscard]] bool exists(const IdType &id, const AABB &old_bound) const {
				if (!boundary.contains(old_bound)) {
					return false;
				}
				for (const auto &entry : entries) {
					if (entry.component_id == id) {
						return true;
					}
				}
				if (is_leaf()) {
					return false;
				}
				const auto quadrant = containing_quadrant(old_bound);
				return quadrant.has_value() && child(*quadrant)->exists(id, old_bound);
			}

			[[nodiscard]] std::vector<IdType> find(const AABB &area) const {
				std::vector<const Entry<IdType>*> found;
				find_impl(area, found);
				std::vector<IdType> result;
				result.reserve(found.size());
				for (const auto* entry : found) {
					result.push_back(entry->component_id);
				}
				return result;
			}

			[[nodiscard]] bool invariants_hold(std::unordered_set<IdType> &ids, std::size_t &entry_count) const {
				if (depth > MAX_DEPTH) {
					return false;
				}
				for (const auto &entry : entries) {
					if (!boundary.contains(entry.bounds) || !ids.insert(entry.component_id).second) {
						return false;
					}
					++entry_count;
				}
				for (const auto quadrant : ALL_QUADRANTS) {
					const auto &node = child(quadrant);
					if (node
						&& (!boundary.contains(node->boundary) || node->depth != depth + 1
							|| !node->invariants_hold(ids, entry_count))) {
						return false;
					}
				}
				return true;
			}

			[[nodiscard]] std::optional<IdType> if_find_one_stop(const AABB &area) const {
				std::vector<const Entry<IdType>*> found;
				find_impl(area, found);
				if (found.empty()) {
					return std::nullopt;
				}
				return found.front()->component_id;
			}
	};

	template <typename IdType> class Quadtree {
		private:

			std::unique_ptr<Node<IdType>> root_;
			std::size_t                   size_ = 0;

		public:

			explicit Quadtree(const AABB &boundary)
				: root_(std::make_unique<Node<IdType>>(boundary)) {}

			[[nodiscard]] std::size_t size() const noexcept {
				return size_;
			}

			[[nodiscard]] const Node<IdType> &root() const noexcept {
				return *root_;
			}

			[[nodiscard]] Node<IdType> &root() noexcept {
				return *root_;
			}

			bool add(const IdType &id, const AABB &bound) {
				if (root_->exists(id) || !root_->insert(Entry<IdType>{id, bound})) {
					return false;
				}
				++size_;
				return true;
			}

			bool remove(const IdType &id) {
				if (!root_->remove(id)) {
					return false;
				}
				--size_;
				return true;
			}

			bool remove(const IdType &id, const AABB &old_bound) {
				if (!root_->remove(id, old_bound)) {
					return false;
				}
				--size_;
				return true;
			}

			UpdateOutput update(const IdType &id, const AABB &new_bound) {
				const auto result = root_->update(id, new_bound);
				if (result == UpdateOutput::EXISTS_BUT_WAS_REMOVED) {
					--size_;
				}
				return result;
			}

			UpdateOutput update(const IdType &id, const AABB &new_bound, const AABB &old_bound) {
				const auto result = root_->update(id, new_bound, old_bound);
				if (result == UpdateOutput::EXISTS_BUT_WAS_REMOVED) {
					--size_;
				}
				return result;
			}

			[[nodiscard]] bool exists(const IdType &id) const {
				return root_->exists(id);
			}

			[[nodiscard]] bool exists(const IdType &id, const AABB &bound) const {
				return root_->exists(id, bound);
			}

			[[nodiscard]] std::vector<IdType> find(const AABB &area) const {
				return root_->find(area);
			}

			[[nodiscard]] std::optional<IdType> if_find_one_stop(const AABB &area) const {
				return root_->if_find_one_stop(area);
			}

			[[nodiscard]] bool invariants_hold() const {
				std::unordered_set<IdType> ids;
				std::size_t                entry_count = 0;
				return root_->invariants_hold(ids, entry_count) && entry_count == size_;
			}
	};

} // namespace KShared::Functional::Quadtree
