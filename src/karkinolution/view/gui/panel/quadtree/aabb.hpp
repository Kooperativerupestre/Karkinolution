#pragma once

#include "karkinolution/view/gui/geometry/geometry.hpp"

namespace KarkinolutionView {

	namespace Gui {
		namespace Panel {
			namespace Quadtree {

				using Geometry::Position;

				class AABB {
					public:

						Position min;
						Position max;

						constexpr AABB(Position min, Position max)
							: min(min)
							, max(max) {}

						constexpr bool contains(const Position &point) const {
							return point.x >= min.x && point.x <= max.x && point.y >= min.y
								&& point.y <= max.y;
						}

						constexpr bool contains(const AABB &other) const {
							return other.min.x >= min.x && other.max.x <= max.x
								&& other.min.y >= min.y && other.max.y <= max.y;
						}

						constexpr bool intersects(const AABB &other) const {
							return min.x <= other.max.x && max.x >= other.min.x
								&& min.y <= other.max.y && max.y >= other.min.y;
						}
				};

				constexpr AABB to_aabb(const Gui::Geometry::Rectangle &rectangle) {
					const float half_width  = rectangle.width.value() / 2.0f;
					const float half_height = rectangle.height.value() / 2.0f;

					Position min = rectangle.position;
					Position max = rectangle.position;

					min = min.move_to_left(half_width);
					min = min.move_up(half_height);

					max = max.move_to_right(half_width);
					max = max.move_down(half_height);

					return {min, max};
				}

				constexpr AABB to_aabb(const Geometry::Circle &circle) {
					Position min = circle.position;
					Position max = circle.position;

					min = min.move_to_left(circle.radius.value());
					min = min.move_up(circle.radius.value());

					max = max.move_to_right(circle.radius.value());
					max = max.move_down(circle.radius.value());

					return {min, max};
				}
			} // namespace Quadtree
		} // namespace Panel
	} // namespace Gui
} // namespace KarkinolutionView