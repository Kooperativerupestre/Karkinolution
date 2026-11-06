#pragma once

#include "karkinolution/functional/quadtree.hpp"
#include "karkinolution/view/gui/components/identity.hpp"
#include "karkinolution/view/gui/geometry/geometry.hpp"

#include <cstddef>
#include <optional>
#include <vector>

namespace KarkinolutionView::Gui::Panel::Quadtree {

	using UpdateOutput = Karkinolution::Functional::Quadtree::UpdateOutput;

	class Quadtree {
		private:

			using FunctionalTree = Karkinolution::Functional::Quadtree::Quadtree<Components::ComponentId>;
			FunctionalTree tree_;

			static Karkinolution::Functional::Quadtree::AABB to_aabb(const Geometry::Rectangle &rectangle) {
				const auto half_width  = static_cast<float>(rectangle.width.value()) / 2.0f;
				const auto half_height = static_cast<float>(rectangle.height.value()) / 2.0f;
				return {{rectangle.position.x - half_width, rectangle.position.y - half_height},
						{rectangle.position.x + half_width, rectangle.position.y + half_height}};
			}

			static Karkinolution::Functional::Quadtree::AABB to_aabb(const Geometry::Circle &circle) {
				const auto radius = circle.radius.value();
				return {{circle.position.x - radius, circle.position.y - radius},
						{circle.position.x + radius, circle.position.y + radius}};
			}

		public:

			explicit Quadtree(const Geometry::Rectangle &boundary)
				: tree_(to_aabb(boundary)) {}

			explicit Quadtree(const Geometry::Circle &boundary)
				: tree_(to_aabb(boundary)) {}

			[[nodiscard]] std::size_t size() const noexcept {
				return tree_.size();
			}

			[[nodiscard]] bool invariants_hold() const {
				return tree_.invariants_hold();
			}

			bool add(Components::ComponentId id, const Geometry::Rectangle &form) {
				return tree_.add(id, to_aabb(form));
			}

			bool add(Components::ComponentId id, const Geometry::Circle &form) {
				return tree_.add(id, to_aabb(form));
			}

			bool remove(Components::ComponentId id) {
				return tree_.remove(id);
			}

			bool remove(Components::ComponentId id, const Geometry::Rectangle &form) {
				return tree_.remove(id, to_aabb(form));
			}

			bool remove(Components::ComponentId id, const Geometry::Circle &form) {
				return tree_.remove(id, to_aabb(form));
			}

			UpdateOutput update(Components::ComponentId id, const Geometry::Rectangle &form) {
				return tree_.update(id, to_aabb(form));
			}

			UpdateOutput
			update(Components::ComponentId id, const Geometry::Rectangle &form, const Geometry::Rectangle &old_form) {
				return tree_.update(id, to_aabb(form), to_aabb(old_form));
			}

			UpdateOutput update(Components::ComponentId id, const Geometry::Circle &form) {
				return tree_.update(id, to_aabb(form));
			}

			UpdateOutput
			update(Components::ComponentId id, const Geometry::Circle &form, const Geometry::Circle &old_form) {
				return tree_.update(id, to_aabb(form), to_aabb(old_form));
			}

			[[nodiscard]] bool exists(Components::ComponentId id) const {
				return tree_.exists(id);
			}

			[[nodiscard]] bool exists(Components::ComponentId id, const Geometry::Rectangle &form) const {
				return tree_.exists(id, to_aabb(form));
			}

			[[nodiscard]] bool exists(Components::ComponentId id, const Geometry::Circle &form) const {
				return tree_.exists(id, to_aabb(form));
			}

			[[nodiscard]] std::vector<Components::ComponentId> find_in_area(const Geometry::Rectangle &area) const {
				return tree_.find(to_aabb(area));
			}

			[[nodiscard]] std::vector<Components::ComponentId> find_in_area(const Geometry::Circle &area) const {
				return tree_.find(to_aabb(area));
			}

			[[nodiscard]] std::optional<Components::ComponentId>
			if_find_one_stop(const Geometry::Rectangle &area) const {
				return tree_.if_find_one_stop(to_aabb(area));
			}

			[[nodiscard]] std::optional<Components::ComponentId> if_find_one_stop(const Geometry::Circle &area) const {
				return tree_.if_find_one_stop(to_aabb(area));
			}
	};

} // namespace KarkinolutionView::Gui::Panel::Quadtree
