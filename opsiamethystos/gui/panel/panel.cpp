#include "opsiamethystos/gui/panel/panel.hpp"

#include <stdexcept>
#include <unordered_set>
#include <utility>

namespace Opsiamethystos {
	namespace Gui {
		namespace Panel {
			// Add is deliberately a two-phase ownership operation: the spatial index
			// must accept the component before the map claims ownership. If map insertion
			// unexpectedly fails, remove restores the pre-call state.
			bool Panel::add(std::shared_ptr<Components::Component> component) {
				if (!component) {
					return false;
				}

				const auto id = component->get_id();
				if (components_.contains(id)) {
					return false;
				}

				components_.reserve(components_.size() + 1);

				const auto index_bounds = component->get_index_bounds();
				const bool indexed      = quadtree_.add(id, index_bounds);
				if (!indexed) {
					return false;
				}

				auto result = components_.emplace(id, Entry{std::move(component), index_bounds});
				if (!result.second) {
					if (!quadtree_.remove(id)) {
						throw std::logic_error("Failed to roll back component insertion");
					}
					return false;
				}

				layout_dirty_ = true;
				return true;
			}

			// Removing the map entry first would lose the bound needed to remove the
			// matching quadtree record, so the index is retired before ownership.
			bool Panel::remove(Components::ComponentId id) {
				const auto component = components_.find(id);
				if (component == components_.end()) {
					return false;
				}

				if (!quadtree_.remove(id)) {
					return false;
				}

				components_.erase(component);
				layout_dirty_ = true;
				return true;
			}

			// Layout changes are cheap to stage and potentially expensive to apply;
			// defer the pass so callers can edit the tree without partial updates.
			void Panel::set_layout(Layout::Node layout_root) {
				layout_root_  = std::move(layout_root);
				layout_dirty_ = true;
			}

			void Panel::mark_layout_dirty() noexcept {
				layout_dirty_ = true;
			}

			// The validation pass prevents a malformed tree from changing only part of
			// the panel. The application pass then reuses set_bounds for each atomic
			// component/index update and records old slots for reverse rollback.
			LayoutApplyResult Panel::relayout() {
				if (!layout_dirty_ || !layout_root_) {
					return {};
				}

				const Layout::Edges root_edges{bounds_.left().value,
											   bounds_.top().value,
											   bounds_.right().value,
											   bounds_.floor().value};
				const auto          result = Layout::layout(root_edges, *layout_root_);

				for (const auto &assignment : result.assignments) {
					const auto entry = components_.find(assignment.id);
					if (entry == components_.end() || !Geometry::can_contain(bounds_, assignment.slot)) {
						return {.applied = false, .overflowed = result.overflowed};
					}
				}

				std::vector<std::pair<Components::ComponentId, Geometry::Rectangle>> applied;
				for (const auto &assignment : result.assignments) {
					const auto entry = components_.find(assignment.id);
					if (entry->second.indexed_bound == assignment.slot) {
						continue;
					}

					const auto old_slot = entry->second.indexed_bound;
					if (!set_bounds(assignment.id, assignment.slot)) {
						for (auto rollback = applied.rbegin(); rollback != applied.rend(); ++rollback) {
							if (!set_bounds(rollback->first, rollback->second)) {
								throw std::logic_error("Failed to roll back layout batch");
							}
						}
						return {.applied = false, .overflowed = result.overflowed};
					}
					applied.emplace_back(assignment.id, old_slot);
				}

				layout_dirty_ = false;
				return {.applied = true, .overflowed = result.overflowed};
			}

			// Quadtree::update removes the old record before inserting the new one.
			// Therefore the Entry::indexed_bound value, rather than component geometry,
			// is always used for both the update and any recovery operation.
			bool Panel::set_bounds(Components::ComponentId id, const Geometry::Rectangle &slot) {
				const auto entry = components_.find(id);
				if (entry == components_.end() || !Geometry::can_contain(bounds_, slot)) {
					return false;
				}

				const auto update = quadtree_.update(id, slot, entry->second.indexed_bound);

				if (update == Quadtree::UpdateOutput::DOES_NOT_EXIST) {
					return false;
				}
				if (update == Quadtree::UpdateOutput::EXISTS_BUT_WAS_REMOVED) {
					const bool restored = quadtree_.add(id, entry->second.indexed_bound);
					if (!restored) {
						throw std::logic_error("Failed to restore removed component bound");
					}
					return false;
				}

				if (!entry->second.component->set_slot(slot)) {
					const bool rolled_back =
						quadtree_.update(id, entry->second.indexed_bound, slot) == Quadtree::UpdateOutput::UPDATED;
					if (!rolled_back) {
						throw std::logic_error("Failed to roll back component bound update");
					}
					return false;
				}

				entry->second.indexed_bound = slot;
				return true;
			}

			// Resize computes all targets before mutating anything. Component updates are
			// staged first, then a replacement quadtree is built off to the side; the
			// live boundary/index pair changes only after both are known to be valid.
			// The argument is a parent-space rectangle, so its position becomes the new
			// origin while the replacement boundary is normalized back to local space.
			bool Panel::resize(const Geometry::Rectangle &bounds) {
				const Geometry::Rectangle local_bounds{{0, 0}, bounds.width, bounds.height};
				std::unordered_map<Components::ComponentId, Geometry::Rectangle> target_bounds;
				target_bounds.reserve(components_.size());

				if (layout_root_) {
					const Layout::Edges                         edges{local_bounds.left().value,
                                              local_bounds.top().value,
                                              local_bounds.right().value,
                                              local_bounds.floor().value};
					const auto                                  result = Layout::layout(edges, *layout_root_);
					std::unordered_set<Components::ComponentId> assigned_ids;
					for (const auto &assignment : result.assignments) {
						if (!components_.contains(assignment.id)
							|| !Geometry::can_contain(local_bounds, assignment.slot)
							|| !assigned_ids.insert(assignment.id).second) {
							return false;
						}
						target_bounds.emplace(assignment.id, assignment.slot);
					}
				}

				for (const auto &[id, entry] : components_) {
					if (!target_bounds.contains(id)) {
						if (!Geometry::can_contain(local_bounds, entry.indexed_bound)) {
							return false;
						}
						target_bounds.emplace(id, entry.indexed_bound);
					}
				}

				std::vector<std::pair<Components::ComponentId, Geometry::Rectangle>> changed;
				for (const auto &[id, target] : target_bounds) {
					const auto entry = components_.find(id);
					if (entry->second.indexed_bound == target) {
						continue;
					}
					const auto old = entry->second.indexed_bound;
					if (!entry->second.component->set_slot(target)) {
						for (auto rollback = changed.rbegin(); rollback != changed.rend(); ++rollback) {
							if (!components_.at(rollback->first).component->set_slot(rollback->second)) {
								throw std::logic_error("Failed to roll back component resize");
							}
						}
						return false;
					}
					changed.emplace_back(id, old);
				}

				Quadtree::Quadtree replacement{local_bounds};
				for (const auto &[id, target] : target_bounds) {
					if (!replacement.add(id, target)) {
						for (auto rollback = changed.rbegin(); rollback != changed.rend(); ++rollback) {
							if (!components_.at(rollback->first).component->set_slot(rollback->second)) {
								throw std::logic_error("Failed to roll back component resize");
							}
						}
						return false;
					}
				}

				for (const auto &[id, target] : target_bounds) {
					components_.at(id).indexed_bound = target;
				}
				quadtree_     = std::move(replacement);
				bounds_       = local_bounds;
				origin_       = bounds.position;
				layout_dirty_ = false;
				return true;
			}

			// Parent and local spaces differ only by translation. Keeping this conversion
			// centralized prevents callers from accidentally applying the origin twice.
			Geometry::Rectangle Panel::to_local(const Geometry::Rectangle &area) const {
				return {area.position - origin_, area.width, area.height};
			}

			Geometry::Circle Panel::to_local(const Geometry::Circle &area) const {
				return {area.position - origin_, area.radius};
			}

			// The panel returns the shared handle so the screen can render the component;
			// mutation of its slot remains private to Panel through Component::set_slot.
			std::shared_ptr<Components::Component> Panel::get(Components::ComponentId id) const {
				const auto component = components_.find(id);
				if (component == components_.end()) {
					return {};
				}
				return component->second.component;
			}

			// These helpers expose only broad-phase candidates. Exact shape checks are
			// intentionally performed by intersects in the next stage.
			std::vector<Components::ComponentId> Panel::find_in_area(const Geometry::Rectangle &area) const {
				return quadtree_.find_in_area(area);
			}

			std::vector<Components::ComponentId> Panel::find_in_area(const Geometry::Circle &area) const {
				return quadtree_.find_in_area(area);
			}

			// A rectangular quadtree bound is conservative for circles and lines, so
			// candidate retrieval alone cannot define collision semantics.
			template <typename Area> bool Panel::intersects(Components::ComponentId id, const Area &area) const {
				const auto component = get(id);
				if (!component) {
					return false;
				}

				return std::visit(
					[&area](const auto &component_bound) {
						return Geometry::intersects(component_bound, area);
					},
					component->get_bounds());
			}

			// Narrowing the broad-phase candidates here gives callers exact collisions
			// without coupling the quadtree to every component shape.
			template <typename Area>
			std::vector<Components::ComponentId> Panel::collides_in_area(const Area &area) const {
				std::vector<Components::ComponentId> collisions;
				for (const auto id : find_in_area(area)) {
					if (intersects(id, area)) {
						collisions.push_back(id);
					}
				}
				return collisions;
			}

			std::vector<Components::ComponentId> Panel::collides(const Geometry::Rectangle &area) const {
				return collides_in_area(area);
			}

			std::vector<Components::ComponentId> Panel::collides(const Geometry::Circle &area) const {
				return collides_in_area(area);
			}

			std::vector<Components::ComponentId> Panel::collides_in_parent(const Geometry::Rectangle &area) const {
				return collides(to_local(area));
			}

			std::vector<Components::ComponentId> Panel::collides_in_parent(const Geometry::Circle &area) const {
				return collides(to_local(area));
			}

			// Stop after the first exact match, preserving quadtree traversal order and
			// avoiding a false promise that the result is nearest or visually foremost.
			template <typename Area>
			std::optional<Components::ComponentId> Panel::find_one_collision_in_area_impl(const Area &area) const {
				for (const auto id : find_in_area(area)) {
					if (intersects(id, area)) {
						return id;
					}
				}
				return std::nullopt;
			}

			std::optional<Components::ComponentId>
			Panel::find_one_collision_in_area(const Geometry::Rectangle &area) const {
				return find_one_collision_in_area_impl(area);
			}

			std::optional<Components::ComponentId>
			Panel::find_one_collision_in_area(const Geometry::Circle &area) const {
				return find_one_collision_in_area_impl(area);
			}

			std::optional<Components::ComponentId>
			Panel::find_one_collision_in_parent(const Geometry::Rectangle &area) const {
				return find_one_collision_in_area(to_local(area));
			}

			std::optional<Components::ComponentId>
			Panel::find_one_collision_in_parent(const Geometry::Circle &area) const {
				return find_one_collision_in_area(to_local(area));
			}
		} // namespace Panel
	} // namespace Gui
} // namespace Opsiamethystos
