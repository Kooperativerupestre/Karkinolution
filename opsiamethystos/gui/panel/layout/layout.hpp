#pragma once

#include "opsiamethystos/gui/components/identity.hpp"
#include "opsiamethystos/math/geometry/geometry.hpp"

#include <functional>
#include <memory>
#include <variant>
#include <vector>

namespace Opsiamethystos {
	namespace Gui {
		namespace Panel {
			namespace Layout {
				// Layout works with edges instead of center-plus-extent rectangles. This
				// keeps stacking arithmetic independent from the geometry conversion used
				// by the rest of the GUI.
				struct Edges {
						double left;
						double top;
						double right;
						double floor;
				};

				struct Insets {
						double left{};
						double top{};
						double right{};
						double floor{};
				};

				struct Size {
						// Size is the preferred, unallocated extent of a leaf. It is not the
						// slot the leaf receives; the parent stack decides that slot.
						double width{};
						double height{};
				};

				enum class CrossAlign : uint8_t {
					START,
					CENTER,
					END,
					STRETCH
				};
				enum class Direction : uint8_t {
					VERTICAL,
					HORIZONTAL
				};

				// Fixed and content policies consume space before fill policies. Fill
				// policies share whatever remains after those mandatory sizes and gaps.
				struct Fixed {
						double extent;
				};

				struct Content {};

				struct Fill {
						double weight = 1.0;
				};

				using SizePolicy = std::variant<Fixed, Content, Fill>;
				// A measurement callback supplies intrinsic content size in GUI units.
				// The layout invokes it only for Content policies and for cross-axis
				// preferred sizing. It must be deterministic and must not mutate the
				// component or depend on rendering/frame state. Non-positive results are
				// normalized to the layout minimum by the caller.
				using Measure = std::function<Size()>;

				// A leaf is the only node that produces an assignment. Containers only
				// describe how their descendants receive space and are never indexed.
				struct Leaf {
						Components::ComponentId id;
						Measure                 measure;
				};

				struct Node;

				struct Child {
						SizePolicy                  policy;
						std::shared_ptr<const Node> node;
				};

				// A stack distributes children along one main axis and aligns them on
				// the other axis. The same representation supports vertical and
				// horizontal layout without duplicating the algorithm.
				struct Stack {
						Direction          direction;
						Insets             padding;
						double             spacing{};
						CrossAlign         cross_align = CrossAlign::START;
						std::vector<Child> children;
				};

				struct Node {
						std::variant<Leaf, Stack> value;
				};

				struct Assignment {
						Components::ComponentId id;
						Geometry::Rectangle     slot;
				};

				struct LayoutResult {
						// Assignments are emitted in tree/child order so the panel can apply
						// them as one deterministic transaction.
						std::vector<Assignment> assignments;
						// True means the requested preferred/fixed arrangement did not fit;
						// slots are still clamped to valid geometry for transactional handling.
						bool overflowed = false;
				};

				// Performs a deterministic pure layout pass. The root box is expressed in
				// edge coordinates, and the result contains one rectangular slot per leaf.
				// It does not know about the panel, quadtree, component mutation, rendering,
				// or frame state; the panel owns applying the returned assignments.
				[[nodiscard]] LayoutResult layout(const Edges &box, const Node &root);
			} // namespace Layout
		} // namespace Panel
	} // namespace Gui
} // namespace Opsiamethystos
