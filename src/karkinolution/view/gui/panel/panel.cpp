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

				std::unordered_map<Components::ComponentId, std::shared_ptr<Components::Component>>
					staged_component;
				staged_component.emplace(id, std::move(component));
				components_.reserve(components_.size() + 1);

				const auto &staged  = staged_component.at(id);
				const auto  bounds  = staged->get_bounds();
				const bool  indexed = std::visit(
                    [this, id](const auto &bound) {
                        return quadtree_.add(id, bound);
                    },
                    bounds);
				if (!indexed) {
					return false;
				}

				auto node   = staged_component.extract(id);
				auto result = components_.insert(std::move(node));
				if (!result.inserted) {
					if (!quadtree_.remove(id)) {
						throw std::logic_error("Failed to roll back component insertion");
					}
					return false;
				}

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
				return true;
			}

			std::shared_ptr<Components::Component> Panel::get(Components::ComponentId id) const {
				const auto component = components_.find(id);
				if (component == components_.end()) {
					return {};
				}
				return component->second;
			}

			std::vector<Components::ComponentId>
			Panel::find_in_area(const Geometry::Rectangle &area) const {
				return quadtree_.find_in_area(area);
			}

			std::vector<Components::ComponentId>
			Panel::find_in_area(const Geometry::Circle &area) const {
				return quadtree_.find_in_area(area);
			}

			template <typename Area>
			bool Panel::intersects(Components::ComponentId id, const Area &area) const {
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

			std::vector<Components::ComponentId>
			Panel::collides(const Geometry::Rectangle &area) const {
				return collides_in_area(area);
			}

			std::vector<Components::ComponentId>
			Panel::collides(const Geometry::Circle &area) const {
				return collides_in_area(area);
			}

			template <typename Area>
			std::optional<Components::ComponentId>
			Panel::find_one_collision_in_area_impl(const Area &area) const {
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
