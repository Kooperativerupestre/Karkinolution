#pragma once

#include "karkinolution/view/gui/components/identity.hpp"
#include "karkinolution/view/gui/geometry/geometry.hpp"
#include "karkinolution/view/gui/panel/quadtree/aabb.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <utility>
#include <vector>

namespace KarkinolutionView {
	namespace Gui {
		namespace Panel {

			namespace Quadtree {
				using Gui::Components::ComponentId;

				class QuadtreeTestAccess;

				enum class Quadrant : uint8_t {
					UPPER_LEFT,
					UPPER_RIGHT,
					LOWER_LEFT,
					LOWER_RIGHT
				};

				struct Entry {
						ComponentId component_id;
						AABB        bounds;
				};

				inline constexpr size_t                  QUADTREE_CHILDREN_TRESHOLD = 8;
				inline constexpr size_t                  MAX_ENTRIES                = 8;
				inline constexpr std::size_t             MAX_DEPTH                  = 8;
				inline constexpr std::array<Quadrant, 4> all_quadrants{Quadrant::LOWER_LEFT,
																	   Quadrant::LOWER_RIGHT,
																	   Quadrant::UPPER_LEFT,
																	   Quadrant::UPPER_RIGHT};
				enum class UpdateOutput : uint8_t {
					DOES_NOT_EXIST,
					UPDATED,
					EXISTS_BUT_WAS_REMOVED
				};

				class Node {
					private:

						friend class QuadtreeTestAccess;

						AABB boundary;

						std::unique_ptr<Node> upper_left;
						std::unique_ptr<Node> upper_right;
						std::unique_ptr<Node> lower_left;
						std::unique_ptr<Node> lower_right;

						std::vector<Entry> entries;

						std::size_t depth;

						std::unique_ptr<Node>       &get_quadrant(Quadrant quadrant);
						const std::unique_ptr<Node> &get_quadrant(Quadrant quadrant) const;

						AABB calculate_quadrant_boundary(Quadrant quadrant) const;

						void find(const AABB                &area,
								  std::vector<const Entry*> &output_entries) const;

					public:

						explicit Node(const AABB &aabb, std::size_t depth = 0);
						~Node();

						Node(const Node &)            = delete;
						Node &operator=(const Node &) = delete;

						Node(Node &&) noexcept;
						Node &operator=(Node &&) noexcept;

						bool can_contain(const AABB &aabb) const;
						bool can_contain(const Position &position) const;

						std::optional<Quadrant> get_containing_quadrant(const AABB &aabb) const;

						bool is_leaf() const;
						bool is_full() const;
						bool is_at_deepest_height() const;

						bool subdivide();

						std::optional<Quadrant> get_child_containing(const AABB &bound) const;

						bool insert(const Entry &entry);

						bool remove(ComponentId id);
						bool remove(ComponentId id, const AABB &old_box);

						UpdateOutput update(ComponentId id, const AABB &new_box);
						UpdateOutput
						update(ComponentId id, const AABB &new_box, const AABB &old_box);

						bool exists(ComponentId id) const;
						bool exists(ComponentId id, const AABB &old_aabb) const;

						/*
						Functions that accept an additional `old_box` parameter are generally faster
						than their counterparts without it.

						Providing the component's previous AABB allows the node to narrow down the
						search path, avoiding unnecessary traversal of unrelated quadrants.

						However, these functions require the exact previous AABB of the component.
						If the provided AABB is incorrect or unavailable, they will not fall back to
						a full-tree search. Instead, they may return `false`, even if the component
						exists in the tree.
						*/
						std::vector<ComponentId> find(const AABB &area) const;

						std::optional<ComponentId> if_find_one_stop(const AABB &area) const;
				};

			} // namespace Quadtree

		} // namespace Panel
	} // namespace Gui
} // namespace KarkinolutionView