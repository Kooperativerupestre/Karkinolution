#include "karkinolution/view/gui/panel/quadtree/node.hpp"

#include <utility>

namespace KarkinolutionView {
	namespace Gui {
		namespace Panel {

			namespace Quadtree {
				Node::Node(const AABB &aabb, std::size_t depth)
					: boundary(aabb)
					, depth(depth) {}

				Node::~Node() = default;

				Node::Node(Node &&) noexcept            = default;
				Node &Node::operator=(Node &&) noexcept = default;

				// ============================================================
				// Quadrants
				// ============================================================

				std::unique_ptr<Node> &Node::get_quadrant(Quadrant quadrant) {
					switch (quadrant) {
						case Quadrant::LOWER_LEFT:
							return lower_left;

						case Quadrant::LOWER_RIGHT:
							return lower_right;

						case Quadrant::UPPER_LEFT:
							return upper_left;

						case Quadrant::UPPER_RIGHT:
							return upper_right;
					}

					std::unreachable();
				}

				const std::unique_ptr<Node> &Node::get_quadrant(Quadrant quadrant) const {
					switch (quadrant) {
						case Quadrant::LOWER_LEFT:
							return lower_left;

						case Quadrant::LOWER_RIGHT:
							return lower_right;

						case Quadrant::UPPER_LEFT:
							return upper_left;

						case Quadrant::UPPER_RIGHT:
							return upper_right;
					}

					std::unreachable();
				}

				AABB Node::calculate_quadrant_boundary(Quadrant quadrant) const {
					const Position center{(boundary.min.x + boundary.max.x) / 2.0f,
										  (boundary.min.y + boundary.max.y) / 2.0f};

					switch (quadrant) {
						case Quadrant::UPPER_LEFT:
							return {boundary.min, center};

						case Quadrant::UPPER_RIGHT:
							return {{center.x, boundary.min.y}, {boundary.max.x, center.y}};

						case Quadrant::LOWER_LEFT:
							return {{boundary.min.x, center.y}, {center.x, boundary.max.y}};

						case Quadrant::LOWER_RIGHT:
							return {center, boundary.max};
					}

					std::unreachable();
				}

				// ============================================================
				// Containment
				// ============================================================

				bool Node::can_contain(const AABB &aabb) const {
					return boundary.contains(aabb);
				}

				bool Node::can_contain(const Position &position) const {
					return boundary.contains(position);
				}

				std::optional<Quadrant> Node::get_containing_quadrant(const AABB &aabb) const {
					for (const Quadrant quadrant : all_quadrants) {
						const auto box = calculate_quadrant_boundary(quadrant);

						if (box.contains(aabb)) {
							return quadrant;
						}
					}

					return std::nullopt;
				}

				bool Node::is_leaf() const {
					return lower_left == nullptr && lower_right == nullptr && upper_left == nullptr
						&& upper_right == nullptr;
				}

				bool Node::is_full() const {
					return entries.size() >= QUADTREE_CHILDREN_TRESHOLD;
				}

				bool Node::is_at_deepest_height() const {
					return depth >= MAX_DEPTH;
				}

				std::optional<Quadrant> Node::get_child_containing(const AABB &bound) const {
					if (is_leaf()) {
						return std::nullopt;
					}

					for (const Quadrant quadrant : all_quadrants) {
						if (get_quadrant(quadrant)->can_contain(bound)) {
							return quadrant;
						}
					}

					return std::nullopt;
				}

				// ============================================================
				// Subdivision
				// ============================================================

				bool Node::subdivide() {
					if (is_at_deepest_height()) {
						return false;
					}

					for (const Quadrant quadrant : all_quadrants) {
						get_quadrant(quadrant) =
							std::make_unique<Node>(calculate_quadrant_boundary(quadrant),
												   depth + 1);
					}

					return true;
				}

				// ============================================================
				// Insertion
				// ============================================================

				bool Node::insert(const Entry &entry) {
					if (!can_contain(entry.bounds)) {
						return false;
					}

					if (is_leaf()) {
						if (!is_full()) {
							entries.push_back(entry);
							return true;
						}

						if (!subdivide()) {
							entries.push_back(entry);
							return true;
						}

						// Redistribute existing entries.
						for (std::size_t i = entries.size(); i-- > 0;) {
							const auto quadrant = get_containing_quadrant(entries[i].bounds);

							if (!quadrant.has_value()) {
								continue;
							}

							if (get_quadrant(*quadrant)->insert(entries[i])) {
								entries.erase(entries.begin() + i);
							}
						}

						// Insert the new entry.
						const auto quadrant = get_containing_quadrant(entry.bounds);

						if (quadrant.has_value()) {
							return get_quadrant(*quadrant)->insert(entry);
						}

						entries.push_back(entry);
						return true;
					}

					const auto quadrant = get_containing_quadrant(entry.bounds);

					if (quadrant.has_value()) {
						return get_quadrant(*quadrant)->insert(entry);
					}

					// The entry crosses quadrant boundaries.
					entries.push_back(entry);
					return true;
				}

				// ============================================================
				// Removal
				// ============================================================

				bool Node::remove(ComponentId id) {
					for (std::size_t i = entries.size(); i-- > 0;) {
						if (entries[i].component_id == id) {
							entries.erase(entries.begin() + i);
							return true;
						}
					}

					if (is_leaf()) {
						return false;
					}

					for (const Quadrant quadrant : all_quadrants) {
						if (get_quadrant(quadrant)->remove(id)) {
							return true;
						}
					}

					return false;
				}

				bool Node::remove(ComponentId id, const AABB &old_box) {
					if (!can_contain(old_box)) {
						return false;
					}

					for (std::size_t i = entries.size(); i-- > 0;) {
						if (entries[i].component_id == id) {
							entries.erase(entries.begin() + i);
							return true;
						}
					}

					if (is_leaf()) {
						return false;
					}

					const auto quadrant = get_containing_quadrant(old_box);

					if (!quadrant.has_value()) {
						return false;
					}

					return get_quadrant(*quadrant)->remove(id, old_box);
				}

				// ============================================================
				// Update
				// ============================================================

				UpdateOutput Node::update(ComponentId id, const AABB &new_box) {
					if (!can_contain(new_box)) {
						return UpdateOutput::DOES_NOT_EXIST;
					}

					if (!remove(id)) {
						return UpdateOutput::DOES_NOT_EXIST;
					}

					const auto was_inserted = insert(Entry{.component_id = id, .bounds = new_box});

					if (was_inserted) {
						return UpdateOutput::UPDATED;
					}
					return UpdateOutput::EXISTS_BUT_WAS_REMOVED;
				}

				UpdateOutput
				Node::update(ComponentId id, const AABB &new_box, const AABB &old_box) {
					if (!can_contain(new_box)) {
						return UpdateOutput::DOES_NOT_EXIST;
					}

					if (!remove(id, old_box)) {
						return UpdateOutput::DOES_NOT_EXIST;
					}

					const auto was_inserted = insert(Entry{.component_id = id, .bounds = new_box});
					if (was_inserted) {
						return UpdateOutput::UPDATED;
					}
					return UpdateOutput::EXISTS_BUT_WAS_REMOVED;
				}

				// ============================================================
				// Existence
				// ============================================================

				bool Node::exists(ComponentId id) const {
					for (const auto &entry : entries) {
						if (entry.component_id == id) {
							return true;
						}
					}

					if (is_leaf()) {
						return false;
					}

					for (const Quadrant quadrant : all_quadrants) {
						if (get_quadrant(quadrant)->exists(id)) {
							return true;
						}
					}

					return false;
				}

				bool Node::exists(ComponentId id, const AABB &old_aabb) const {
					if (!can_contain(old_aabb)) {
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

					const auto quadrant = get_containing_quadrant(old_aabb);

					if (!quadrant.has_value()) {
						return false;
					}

					return get_quadrant(*quadrant)->exists(id, old_aabb);
				}

				// ============================================================
				// Search
				// ============================================================

				void Node::find(const AABB &area, std::vector<const Entry*> &output_entries) const {
					if (!boundary.intersects(area)) {
						return;
					}

					for (const auto &entry : entries) {
						if (entry.bounds.intersects(area)) {
							output_entries.push_back(&entry);
						}
					}

					if (is_leaf()) {
						return;
					}

					for (const Quadrant quadrant : all_quadrants) {
						get_quadrant(quadrant)->find(area, output_entries);
					}
				}

				std::vector<ComponentId> Node::find(const AABB &area) const {
					std::vector<const Entry*> found_entries;

					find(area, found_entries);

					std::vector<ComponentId> ids;
					ids.reserve(found_entries.size());

					for (const Entry* entry : found_entries) {
						ids.push_back(entry->component_id);
					}

					return ids;
				}

				std::optional<ComponentId> Node::if_find_one_stop(const AABB &area) const {
					if (!boundary.intersects(area)) {
						return std::nullopt;
					}

					for (const auto &entry : entries) {
						if (entry.bounds.intersects(area)) {
							return entry.component_id;
						}
					}

					if (is_leaf()) {
						return std::nullopt;
					}

					for (const Quadrant quadrant : all_quadrants) {
						const auto found = get_quadrant(quadrant)->if_find_one_stop(area);

						if (found.has_value()) {
							return found;
						}
					}

					return std::nullopt;
				}
			} // namespace Quadtree
		} // namespace Panel
	} // namespace Gui
} // namespace KarkinolutionView
