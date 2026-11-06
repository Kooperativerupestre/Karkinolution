#pragma once

#include "karkinolution/view/gui/components/component.hpp"
#include "karkinolution/view/gui/geometry/geometry.hpp"
#include "karkinolution/view/gui/panel/layout/layout.hpp"
#include "karkinolution/view/gui/panel/quadtree/quadtree.hpp"

#include <memory>
#include <optional>
#include <unordered_map>
#include <vector>

namespace KarkinolutionView {
	namespace Gui {
		namespace Panel {
			struct LayoutApplyResult {
					bool applied    = false;
					bool overflowed = false;
			};

			class Panel {
				private:

					Quadtree::Quadtree          quadtree_;
					Geometry::Rectangle         bounds_;
					std::optional<Layout::Node> layout_root_;
					bool                        layout_dirty_ = false;

					struct Entry {
							std::shared_ptr<Components::Component> component;
							Geometry::Rectangle                    indexed_bound;
					};

					std::unordered_map<Components::ComponentId, Entry> components_;

					std::vector<Components::ComponentId> find_in_area(const Geometry::Rectangle &area) const;

					std::vector<Components::ComponentId> find_in_area(const Geometry::Circle &area) const;

					template <typename Area> bool intersects(Components::ComponentId id, const Area &area) const;

					template <typename Area>
					std::vector<Components::ComponentId> collides_in_area(const Area &area) const;

					template <typename Area>
					std::optional<Components::ComponentId> find_one_collision_in_area_impl(const Area &area) const;

				public:

					explicit Panel(const Geometry::Rectangle &bounds)
						: quadtree_(bounds)
						, bounds_(bounds) {}

					bool add(std::shared_ptr<Components::Component> component);
					bool remove(Components::ComponentId id);

					[[nodiscard]] bool set_bounds(Components::ComponentId id, const Geometry::Rectangle &slot);
					[[nodiscard]] bool resize(const Geometry::Rectangle &bounds);

					void                            set_layout(Layout::Node layout_root);
					void                            mark_layout_dirty() noexcept;
					[[nodiscard]] LayoutApplyResult relayout();

					[[nodiscard]]
					std::shared_ptr<Components::Component> get(Components::ComponentId id) const;

					[[nodiscard]]
					std::vector<Components::ComponentId> collides(const Geometry::Rectangle &area) const;

					[[nodiscard]]
					std::vector<Components::ComponentId> collides(const Geometry::Circle &area) const;

					[[nodiscard]]
					std::optional<Components::ComponentId>
					find_one_collision_in_area(const Geometry::Rectangle &area) const;

					[[nodiscard]]
					std::optional<Components::ComponentId>
					find_one_collision_in_area(const Geometry::Circle &area) const;

					[[nodiscard]]
					std::size_t size() const noexcept {
						return components_.size();
					}
			};
		} // namespace Panel
	} // namespace Gui
} // namespace KarkinolutionView
