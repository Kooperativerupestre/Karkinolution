#pragma once

#include "opsiamethystos/graphics/panel/components/identity.hpp"
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

				// The child takes its preferred size. `minimum` is a floor on both axes, so content
				// that changes (a label that switches text) keeps a stable slot instead of shrinking
				// and growing. The slot is max(preferred size, minimum); a zero minimum changes nothing.
				struct Content {
						Size minimum{};
				};

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

				// What the layout needs to know about anything it places: a stable identity and a
				// preferred size. Components implement this interface, so layout declarations can take
				// them directly while the layout itself never depends on concrete component types.
				class Participant {
					public:

						virtual ~Participant() = default;

						// The identity under which the panel registered the component.
						[[nodiscard]] virtual Components::ComponentId layout_id() const = 0;
						// The extent the participant would like to have. It is read on every layout
						// pass, so it must reflect the participant's current state.
						[[nodiscard]] virtual Size preferred_size() const = 0;

					protected:

						Participant()                               = default;
						Participant(const Participant &)            = default;
						Participant(Participant &&)                 = default;
						Participant &operator=(const Participant &) = default;
						Participant &operator=(Participant &&)      = default;
				};

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
						Components::ComponentId   id;
						Math::Geometry::Rectangle slot;
				};

				struct LayoutResult {
						// Assignments are emitted in tree/child order so the panel can apply
						// them as one deterministic transaction.
						std::vector<Assignment> assignments;
						// True means the requested preferred/fixed arrangement did not fit;
						// slots are still clamped to valid geometry for transactional handling.
						bool overflowed = false;
				};

				// Builds a leaf for a participant. The leaf keeps the participant alive and asks it for
				// its preferred size on every pass. Throws std::invalid_argument for a null participant.
				[[nodiscard]] Node leaf(std::shared_ptr<const Participant> participant);
				// Same, but the leaf reports `size` instead of the participant's preferred size. Use it
				// when the cross-axis extent must not follow the content, for example a button that
				// is always 100 high. Also throws when a dimension is not finite and non-negative.
				[[nodiscard]] Node leaf(std::shared_ptr<const Participant> participant, Size size);

				// Options shared by row() and column().
				struct StackOptions {
						// Space between the stack's edge and its children.
						Insets padding{};
						// Space between two neighbouring children.
						double spacing{};
						// Defaults to STRETCH, unlike a hand-built Stack. A container has no intrinsic
						// size, so any other alignment would collapse a nested row or column to the
						// minimum extent on the cross axis.
						CrossAlign cross_align = CrossAlign::STRETCH;
				};

				// The builders below describe how a stack gives space to one child. fixed() asks for an
				// exact extent on the main axis, content() for the child's preferred size, and fill()
				// for a weighted share of what the other children leave over. Each takes either a
				// participant (typically a component) or an already built node, and throws
				// std::invalid_argument for a null participant or an invalid extent or weight.
				[[nodiscard]] Child fixed(double extent, Node node);
				[[nodiscard]] Child fixed(double extent, std::shared_ptr<const Participant> participant);
				// content() also takes a minimum size to reserve space for content that changes:
				//   content(status_label, {.width = 120})
				// Throws std::invalid_argument when a dimension is not finite and non-negative.
				[[nodiscard]] Child content(Node node, Size minimum = {});
				[[nodiscard]] Child content(std::shared_ptr<const Participant> participant, Size minimum = {});
				[[nodiscard]] Child fill(Node node, double weight = 1.0);
				[[nodiscard]] Child fill(std::shared_ptr<const Participant> participant, double weight = 1.0);

				// A row places its children left to right, a column top to bottom. They throw
				// std::invalid_argument when the result is not a valid declaration (see validate).
				// Example, a label next to a button:
				//   panel.set_layout(row({content(label), fixed(100, button)}, {.spacing = 10}));
				[[nodiscard]] Node row(std::vector<Child> children, StackOptions options = {});
				[[nodiscard]] Node column(std::vector<Child> children, StackOptions options = {});

				// Checks that a tree is a valid declaration. A violation is a programming error, so it
				// throws std::invalid_argument instead of being reported as a runtime result. Rejected:
				// a child without a node, a Fixed extent that is not finite and greater than zero,
				// a Fill weight, spacing or padding that is not finite and non-negative, and a
				// component that appears in more than one leaf.
				void validate(const Node &root);

				// Performs a deterministic pure layout pass. The root box is expressed in
				// edge coordinates, and the result contains one rectangular slot per leaf.
				// It does not know about the panel, quadtree, component mutation, rendering,
				// or frame state; the panel owns applying the returned assignments.
				[[nodiscard]] LayoutResult layout(const Edges &box, const Node &root);
			} // namespace Layout
		} // namespace Panel
	} // namespace Gui
} // namespace Opsiamethystos
