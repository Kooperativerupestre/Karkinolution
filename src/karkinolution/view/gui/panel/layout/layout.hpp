#pragma once

#include "karkinolution/view/gui/components/identity.hpp"
#include "karkinolution/view/gui/geometry/geometry.hpp"

#include <functional>
#include <memory>
#include <variant>
#include <vector>

namespace KarkinolutionView {
	namespace Gui {
		namespace Panel {
			namespace Layout {
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

				struct Fixed {
						double extent;
				};

				struct Content {};

				struct Fill {
						double weight = 1.0;
				};

				using SizePolicy = std::variant<Fixed, Content, Fill>;
				using Measure    = std::function<Size()>;

				struct Leaf {
						Components::ComponentId id;
						Measure                 measure;
				};

				struct Node;

				struct Child {
						SizePolicy                  policy;
						std::shared_ptr<const Node> node;
				};

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
						std::vector<Assignment> assignments;
						bool                    overflowed = false;
				};

				[[nodiscard]] LayoutResult layout(const Edges &box, const Node &root);
			} // namespace Layout
		} // namespace Panel
	} // namespace Gui
} // namespace KarkinolutionView
