#pragma once

#include "opsiamethystos/graphics/panel/components/component.hpp"
#include "opsiamethystos/graphics/panel/layout/layout.hpp"
#include "opsiamethystos/graphics/panel/layout_error.hpp"
#include "opsiamethystos/graphics/panel/quadtree/quadtree.hpp"
#include "opsiamethystos/math/geometry/geometry.hpp"

#include <expected>
#include <memory>
#include <optional>
#include <unordered_map>
#include <vector>

namespace Opsiamethystos {
	namespace Gui {
		namespace Panel {
			// Outcome of a layout pass that did not fail.
			struct LayoutReport {
					// True when a pass ran and every assignment was committed. False means there was
					// nothing to apply: the layout is not pending, or the panel has no layout.
					bool applied = false;
					// True when the requested arrangement did not fit the panel. The slots were clamped
					// to valid geometry and committed anyway, because a clamped layout is still useful.
					bool overflowed = false;
			};

			class Panel {
				private:

					Quadtree::Quadtree          quadtree_;
					Math::Geometry::Rectangle   bounds_;
					Math::Geometry::Position    origin_;
					std::optional<Layout::Node> layout_root_;
					bool                        layout_dirty_ = false;

					struct Entry {
							// The indexed bound is the authoritative old value for quadtree
							// updates. Reading the component's current shape here would make
							// rollback depend on component-specific geometry.
							std::shared_ptr<Components::Component> component;
							Math::Geometry::Rectangle              indexed_bound;
					};

					std::unordered_map<Components::ComponentId, Entry> components_;

					std::vector<Components::ComponentId> find_in_area(const Math::Geometry::Rectangle &area) const;

					std::vector<Components::ComponentId> find_in_area(const Math::Geometry::Circle &area) const;

					// Shared by set_bounds and relayout so both report the same failure reasons.
					[[nodiscard]] std::expected<void, LayoutErrorCode>
					try_set_bounds(Components::ComponentId id, const Math::Geometry::Rectangle &slot);

					template <typename Area> bool intersects(Components::ComponentId id, const Area &area) const;

					template <typename Area>
					std::vector<Components::ComponentId> collides_in_area(const Area &area) const;

					template <typename Area>
					std::optional<Components::ComponentId> find_one_collision_in_area_impl(const Area &area) const;

					[[nodiscard]] Math::Geometry::Rectangle to_local(const Math::Geometry::Rectangle &area) const;
					[[nodiscard]] Math::Geometry::Circle    to_local(const Math::Geometry::Circle &area) const;

				public:

					// The public boundary is supplied in parent coordinates once. The panel
					// keeps only its size locally and stores the center as an origin, so a
					// drag never has to rewrite every component or quadtree entry.
					explicit Panel(const Math::Geometry::Rectangle &bounds)
						: quadtree_(Math::Geometry::Rectangle{{0, 0}, bounds.width, bounds.height})
						, bounds_(Math::Geometry::Rectangle{{0, 0}, bounds.width, bounds.height})
						, origin_(bounds.position) {}

					// Adds the component to both ownership and spatial indexing. The operation
					// succeeds only when both structures contain the same new member.
					bool add(std::shared_ptr<Components::Component> component);
					// Removes the index entry first; ownership is erased only after that
					// removal succeeds, preserving the two-structure invariant.
					bool remove(Components::ComponentId id);

					// Atomically changes one component's rectangular layout slot, updating
					// the component and quadtree together. A rejected component update is
					// rolled back to the indexed bound stored in Entry.
					[[nodiscard]] bool set_bounds(Components::ComponentId id, const Math::Geometry::Rectangle &slot);
					// Resizes the local boundary and rebuilds the index as one transaction.
					// Existing components that would fall outside the new boundary reject
					// the whole resize instead of being silently removed.
					[[nodiscard]] bool resize(const Math::Geometry::Rectangle &bounds);

					// Movement is a transform of the panel in parent space. Local slots and
					// the quadtree intentionally remain unchanged during dragging.
					void drag_by(const Math::Geometry::Position &delta) noexcept {
						origin_ += delta;
					}

					[[nodiscard]] Math::Geometry::Position origin() const noexcept {
						return origin_;
					}

					void draw() const;
					void draw(const Math::Geometry::Position &origin) const;
					// Replaces the pure layout tree and defers component mutation until the
					// next relayout call, allowing multiple edits to be batched. Throws
					// std::invalid_argument for an invalid tree (see Layout::validate); the previous
					// layout is kept in that case.
					void set_layout(Layout::Node layout_root);
					// Marks the deferred layout pass dirty after a measure-affecting change.
					void mark_layout_dirty() noexcept;
					// Applies the pending layout to every component it references. Does nothing when
					// no layout is pending. The pass is atomic: if any component cannot take its slot,
					// every earlier assignment is rolled back and the error names the failing component.
					// A failed pass leaves the layout pending, so the next call retries it.
					[[nodiscard]] std::expected<LayoutReport, LayoutError> relayout();

					// Per-frame entry point for layout: call it once before draw(). It applies whatever
					// changed since the last frame (add, remove, set_layout, mark_layout_dirty) and is
					// cheap when nothing is pending. Reports exactly what relayout() reports. draw() is
					// const and cannot lay out on its own, which is why this call exists.
					[[nodiscard]] std::expected<LayoutReport, LayoutError> update();

					// Returns the owned type-erased component, or an empty pointer for an
					// unknown id. Ownership stays with the panel's map.
					[[nodiscard]]
					std::shared_ptr<Components::Component> get(Components::ComponentId id) const;

					[[nodiscard]]
					// Local-space queries use the quadtree as a broad phase and exact
					// component geometry as the narrow phase.
					std::vector<Components::ComponentId> collides(const Math::Geometry::Rectangle &area) const;

					[[nodiscard]]
					std::vector<Components::ComponentId> collides(const Math::Geometry::Circle &area) const;

					// Parent-space queries apply the inverse origin transform before using
					// the same local collision pipeline.
					[[nodiscard]] std::vector<Components::ComponentId>
					collides_in_parent(const Math::Geometry::Rectangle &area) const;

					[[nodiscard]] std::vector<Components::ComponentId>
					collides_in_parent(const Math::Geometry::Circle &area) const;

					// Returns the first exact collision in quadtree traversal order. It is
					// not a nearest-component or z-order query.
					[[nodiscard]]
					std::optional<Components::ComponentId>
					find_one_collision_in_area(const Math::Geometry::Rectangle &area) const;

					[[nodiscard]]
					std::optional<Components::ComponentId>
					find_one_collision_in_area(const Math::Geometry::Circle &area) const;

					[[nodiscard]] std::optional<Components::ComponentId>
					find_one_collision_in_parent(const Math::Geometry::Rectangle &area) const;

					[[nodiscard]] std::optional<Components::ComponentId>
					find_one_collision_in_parent(const Math::Geometry::Circle &area) const;

					[[nodiscard]]
					std::size_t size() const noexcept {
						return components_.size();
					}
			};
		} // namespace Panel
	} // namespace Gui
} // namespace Opsiamethystos
