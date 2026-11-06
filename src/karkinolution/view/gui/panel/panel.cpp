#include "karkinolution/view/gui/panel/panel.hpp"

#include <stdexcept>
#include <utility>

namespace KarkinolutionView {
	namespace Gui {
		namespace Panel {
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

			void Panel::set_layout(Layout::Node layout_root) {
				layout_root_  = std::move(layout_root);
				layout_dirty_ = true;
			}

			void Panel::mark_layout_dirty() noexcept {
				layout_dirty_ = true;
			}

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

			std::shared_ptr<Components::Component> Panel::get(Components::ComponentId id) const {
				const auto component = components_.find(id);
				if (component == components_.end()) {
					return {};
				}
				return component->second.component;
			}

			std::vector<Components::ComponentId> Panel::find_in_area(const Geometry::Rectangle &area) const {
				return quadtree_.find_in_area(area);
			}

			std::vector<Components::ComponentId> Panel::find_in_area(const Geometry::Circle &area) const {
				return quadtree_.find_in_area(area);
			}

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
		} // namespace Panel
	} // namespace Gui
} // namespace KarkinolutionView
