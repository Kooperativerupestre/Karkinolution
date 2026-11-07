#pragma once

#include "opsiamethystos/gui/components/component.hpp"
#include "opsiamethystos/gui/panel/layout/layout.hpp"
#include "opsiamethystos/gui/panel/quadtree/quadtree.hpp"
#include "opsiamethystos/math/geometry/geometry.hpp"

#include <memory>
#include <optional>
#include <unordered_map>
#include <vector>

namespace Opsiamethystos {
	namespace Gui {
		namespace Panel {
			struct LayoutApplyResult {
					// `applied` describes whether every assignment was committed. Overflow
					// is reported independently because a valid clamped layout can still be
					// useful to the caller.
					bool applied    = false;
					bool overflowed = false;
			};

			class Panel {
				private:

					Quadtree::Quadtree          quadtree_;
					Geometry::Rectangle         bounds_;
					Geometry::Position          origin_;
					std::optional<Layout::Node> layout_root_;
					bool                        layout_dirty_ = false;

					struct Entry {
							// The indexed bound is the authoritative old value for quadtree
							// updates. Reading the component's current shape here would make
							// rollback depend on component-specific geometry.
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

					[[nodiscard]] Geometry::Rectangle to_local(const Geometry::Rectangle &area) const;
					[[nodiscard]] Geometry::Circle    to_local(const Geometry::Circle &area) const;

				public:

					// The public boundary is supplied in parent coordinates once. The panel
					// keeps only its size locally and stores the center as an origin, so a
					// drag never has to rewrite every component or quadtree entry.
					explicit Panel(const Geometry::Rectangle &bounds)
						: quadtree_(Geometry::Rectangle{{0, 0}, bounds.width, bounds.height})
						, bounds_(Geometry::Rectangle{{0, 0}, bounds.width, bounds.height})
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
					[[nodiscard]] bool set_bounds(Components::ComponentId id, const Geometry::Rectangle &slot);
					// Resizes the local boundary and rebuilds the index as one transaction.
					// Existing components that would fall outside the new boundary reject
					// the whole resize instead of being silently removed.
					[[nodiscard]] bool resize(const Geometry::Rectangle &bounds);

					// Movement is a transform of the panel in parent space. Local slots and
					// the quadtree intentionally remain unchanged during dragging.
					void drag_by(const Geometry::Position &delta) noexcept {
						origin_ += delta;
					}

					[[nodiscard]] Geometry::Position origin() const noexcept {
						return origin_;
					}

					// Replaces the pure layout tree and defers component mutation until the
					// next relayout call, allowing multiple edits to be batched.
					void set_layout(Layout::Node layout_root);
					// Marks the deferred layout pass dirty after a measure-affecting change.
					void mark_layout_dirty() noexcept;
					// Computes and atomically applies all assignments. A failed assignment
					// restores every earlier assignment in reverse order.
					[[nodiscard]] LayoutApplyResult relayout();

					// Returns the owned type-erased component, or an empty pointer for an
					// unknown id. Ownership stays with the panel's map.
					[[nodiscard]]
					std::shared_ptr<Components::Component> get(Components::ComponentId id) const;

					[[nodiscard]]
					// Local-space queries use the quadtree as a broad phase and exact
					// component geometry as the narrow phase.
					std::vector<Components::ComponentId> collides(const Geometry::Rectangle &area) const;

					[[nodiscard]]
					std::vector<Components::ComponentId> collides(const Geometry::Circle &area) const;

					// Parent-space queries apply the inverse origin transform before using
					// the same local collision pipeline.
					[[nodiscard]] std::vector<Components::ComponentId>
					collides_in_parent(const Geometry::Rectangle &area) const;

					[[nodiscard]] std::vector<Components::ComponentId>
					collides_in_parent(const Geometry::Circle &area) const;

					// Returns the first exact collision in quadtree traversal order. It is
					// not a nearest-component or z-order query.
					[[nodiscard]]
					std::optional<Components::ComponentId>
					find_one_collision_in_area(const Geometry::Rectangle &area) const;

					[[nodiscard]]
					std::optional<Components::ComponentId>
					find_one_collision_in_area(const Geometry::Circle &area) const;

					[[nodiscard]] std::optional<Components::ComponentId>
					find_one_collision_in_parent(const Geometry::Rectangle &area) const;

					[[nodiscard]] std::optional<Components::ComponentId>
					find_one_collision_in_parent(const Geometry::Circle &area) const;

					[[nodiscard]]
					std::size_t size() const noexcept {
						return components_.size();
					}
			};
		} // namespace Panel
	} // namespace Gui
} // namespace Opsiamethystos
