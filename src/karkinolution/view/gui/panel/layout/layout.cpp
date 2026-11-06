#include "karkinolution/view/gui/panel/layout/layout.hpp"

#include <algorithm>
#include <cmath>
#include <numeric>

namespace KarkinolutionView {
	namespace Gui {
		namespace Panel {
			namespace Layout {
				namespace {
					constexpr double minimum_extent = 1.0;

					struct AxisBox {
							double start;
							double end;
					};

					AxisBox clamp_axis(const AxisBox axis, const AxisBox boundary, bool &overflowed) {
						const double available = axis.end - axis.start;
						if (available >= minimum_extent) {
							return axis;
						}

						overflowed          = true;
						const double center = std::clamp((axis.start + axis.end) / 2.0,
														 boundary.start + minimum_extent / 2.0,
														 boundary.end - minimum_extent / 2.0);
						return {center - minimum_extent / 2.0, center + minimum_extent / 2.0};
					}

					AxisBox clamp_slot(AxisBox slot, const AxisBox container, bool &overflowed) {
						const double available = container.end - container.start;
						if (available < minimum_extent) {
							return clamp_axis(container, container, overflowed);
						}

						if (slot.end <= container.start || slot.start >= container.end) {
							overflowed = true;
							return {container.start, container.start + minimum_extent};
						}

						if (slot.start < container.start) {
							const double extent = std::min(slot.end - slot.start, available);
							slot.start          = container.start;
							slot.end            = container.start + extent;
							overflowed          = true;
						}
						if (slot.end > container.end) {
							const double extent = std::min(slot.end - slot.start, available);
							slot.end            = container.end;
							slot.start          = container.end - extent;
							overflowed          = true;
						}

						if (slot.end - slot.start < minimum_extent) {
							overflowed = true;
							return {slot.start, std::min(slot.start + minimum_extent, container.end)};
						}
						return slot;
					}

					Size measure_node(const Node &node) {
						return std::visit(
							[](const auto &value) -> Size {
								using Value = std::remove_cvref_t<decltype(value)>;
								if constexpr (std::same_as<Value, Leaf>) {
									return value.measure ? value.measure() : Size{};
								} else {
									return {};
								}
							},
							node.value);
					}

					void append_node(const Node &node, const Edges &box, LayoutResult &result);

					void append_stack(const Stack &stack, const Edges &box, LayoutResult &result) {
						const Edges   inner_edges{box.left + stack.padding.left,
                                                box.top + stack.padding.top,
                                                box.right - stack.padding.right,
                                                box.floor - stack.padding.floor};
						const bool    vertical = stack.direction == Direction::VERTICAL;
						const AxisBox main{vertical ? inner_edges.top : inner_edges.left,
										   vertical ? inner_edges.floor : inner_edges.right};
						const AxisBox cross{vertical ? inner_edges.left : inner_edges.top,
											vertical ? inner_edges.right : inner_edges.floor};
						const AxisBox boundary{vertical ? box.top : box.left, vertical ? box.floor : box.right};
						const AxisBox main_box = clamp_axis(main, boundary, result.overflowed);
						const AxisBox cross_box =
							clamp_axis(cross,
									   {vertical ? box.left : box.top, vertical ? box.right : box.floor},
									   result.overflowed);
						const double available = main_box.end - main_box.start;

						std::vector<double> extents(stack.children.size());
						double              fixed_total = 0.0;
						double              fill_weight = 0.0;
						for (std::size_t index = 0; index < stack.children.size(); ++index) {
							const auto &child = stack.children[index];
							std::visit(
								[&](const auto &policy) {
									using Policy = std::remove_cvref_t<decltype(policy)>;
									if constexpr (std::same_as<Policy, Fixed>) {
										extents[index] = std::max(minimum_extent, policy.extent);
										fixed_total += extents[index];
									} else if constexpr (std::same_as<Policy, Content>) {
										const auto measured = child.node ? measure_node(*child.node) : Size{};
										extents[index] =
											std::max(minimum_extent, vertical ? measured.height : measured.width);
										fixed_total += extents[index];
									} else {
										fill_weight += std::max(0.0, policy.weight);
									}
								},
								child.policy);
						}

						const double spacing_total =
							stack.spacing * static_cast<double>(stack.children.empty() ? 0 : stack.children.size() - 1);
						const double remaining = std::max(0.0, available - fixed_total - spacing_total);
						if (fixed_total + spacing_total > available) {
							result.overflowed = true;
						}

						for (std::size_t index = 0; index < stack.children.size(); ++index) {
							if (const auto* fill = std::get_if<Fill>(&stack.children[index].policy)) {
								extents[index] = fill_weight > 0.0
									? remaining * std::max(0.0, fill->weight) / fill_weight
									: minimum_extent;
							}
						}

						double cursor = main_box.start;
						for (std::size_t index = 0; index < stack.children.size(); ++index) {
							const auto  &child           = stack.children[index];
							const double preferred_cross = child.node
								? (vertical ? measure_node(*child.node).width : measure_node(*child.node).height)
								: cross_box.end - cross_box.start;
							AxisBox      child_cross     = cross_box;
							if (stack.cross_align != CrossAlign::STRETCH) {
								const double extent = std::min(preferred_cross, cross_box.end - cross_box.start);
								const double offset = stack.cross_align == CrossAlign::START ? 0.0
									: stack.cross_align == CrossAlign::CENTER
									? (cross_box.end - cross_box.start - extent) / 2.0
									: cross_box.end - cross_box.start - extent;
								child_cross         = {cross_box.start + offset, cross_box.start + offset + extent};
							}

							const AxisBox child_main =
								clamp_slot({cursor, cursor + extents[index]}, main_box, result.overflowed);
							const Edges child_box = vertical
								? Edges{child_cross.start, child_main.start, child_cross.end, child_main.end}
								: Edges{child_main.start, child_cross.start, child_main.end, child_cross.end};
							if (child.node) {
								append_node(*child.node, child_box, result);
							}
							cursor += extents[index] + stack.spacing;
						}
					}

					void append_node(const Node &node, const Edges &box, LayoutResult &result) {
						std::visit(
							[&](const auto &value) {
								using Value = std::remove_cvref_t<decltype(value)>;
								if constexpr (std::same_as<Value, Leaf>) {
									result.assignments.push_back({value.id,
																  Geometry::Rectangle::from_edges(
																	  Geometry::Left{static_cast<float>(box.left)},
																	  Geometry::Top{static_cast<float>(box.top)},
																	  Geometry::Right{static_cast<float>(box.right)},
																	  Geometry::Floor{static_cast<float>(box.floor)})});
								} else {
									append_stack(value, box, result);
								}
							},
							node.value);
					}
				} // namespace

				LayoutResult layout(const Edges &box, const Node &root) {
					LayoutResult result;
					append_node(root, box, result);
					return result;
				}
			} // namespace Layout
		} // namespace Panel
	} // namespace Gui
} // namespace KarkinolutionView
