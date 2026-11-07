#pragma once

#include "opsiamethystos/gui/components/identity.hpp"
#include "opsiamethystos/math/geometry/geometry.hpp"
#include "shared/functional/quadtree.hpp"

#include <cstddef>
#include <optional>
#include <vector>

namespace Opsiamethystos::Gui::Panel::Quadtree {

	using UpdateOutput = KShared::Functional::Quadtree::UpdateOutput;

	class Quadtree {
		private:

			using FunctionalTree = KShared::Functional::Quadtree::Quadtree<Components::ComponentId>;
			FunctionalTree tree_;

			static KShared::Functional::Quadtree::AABB to_aabb(const Geometry::Rectangle &rectangle) {
				// The functional index stores AABBs, so this conversion keeps the GUI
				// geometry convention at the adapter boundary instead of leaking it into
				// the generic spatial structure.
				const auto half_width  = static_cast<float>(rectangle.width.value()) / 2.0f;
				const auto half_height = static_cast<float>(rectangle.height.value()) / 2.0f;
				return {{rectangle.position.x - half_width, rectangle.position.y - half_height},
						{rectangle.position.x + half_width, rectangle.position.y + half_height}};
			}

			static KShared::Functional::Quadtree::AABB to_aabb(const Geometry::Circle &circle) {
				// A circle's AABB is a conservative broad-phase bound. It may produce
				// false-positive candidates, which Panel narrows with exact geometry tests.
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

			// The adapter deliberately exposes shape overloads while the underlying
			// tree remains shape-agnostic and stores only AABBs.
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

			// Callers should prefer the old-bound overload when moving an entry: the
			// panel's indexed_bound is the authoritative location for rollback.
			UpdateOutput update(Components::ComponentId id, const Geometry::Rectangle &form) {
				return tree_.update(id, to_aabb(form));
			}

			// The overload with old_form is required for atomic movement: the generic
			// tree removes by the old spatial location before inserting the new one.
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

			// Searches return broad-phase candidates. Exact shape filtering belongs to
			// Panel because this adapter intentionally knows only AABBs.
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

} // namespace Opsiamethystos::Gui::Panel::Quadtree
