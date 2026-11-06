#pragma once

#include "karkinolution/view/gui/components/identity.hpp"
#include "karkinolution/view/gui/components/low_level/button.hpp"
#include "karkinolution/view/gui/components/low_level/circle.hpp"
#include "karkinolution/view/gui/components/low_level/circle_label.hpp"
#include "karkinolution/view/gui/components/low_level/label.hpp"
#include "karkinolution/view/gui/components/low_level/line.hpp"
#include "karkinolution/view/gui/components/low_level/rectangle.hpp"
#include "karkinolution/view/gui/components/low_level/text.hpp"
#include "karkinolution/view/gui/components/medium_level/grid.hpp"
#include "karkinolution/view/gui/geometry/geometry.hpp"

#include <cstdint>
#include <algorithm>
#include <concepts>
#include <optional>
#include <type_traits>
#include <typeindex>
#include <utility>
#include <variant>

namespace KarkinolutionView::Gui::Panel {
	class Panel;
}

namespace KarkinolutionView {
	namespace Gui {
		namespace Components {
			using AllComponents = std::variant<LowLevel::Button::RectangleButton,
											   LowLevel::Circle,
											   LowLevel::CircleLabel,
											   LowLevel::Line,
											   LowLevel::Rectangle,
											   LowLevel::RectangleLabel,
											   LowLevel::Text,
											   MediumLevel::Grid>;
			using Bound         = std::variant<Geometry::Circle, Geometry::Rectangle>;

			class Component {
				private:

					AllComponents data_;

					bool set_slot(const Geometry::Rectangle &slot) {
						return std::visit(
							[&slot](auto &component) {
								if constexpr (requires { component.set_slot(slot); }) {
									return component.set_slot(slot);
								} else {
									return false;
								}
							},
							data_);
					}

					friend class KarkinolutionView::Gui::Panel::Panel;

				public:

					explicit Component(AllComponents data)
						: data_(std::move(data)) {}

					[[nodiscard]]
					ComponentId get_id() const {
						return std::visit(
							[](const auto &component) {
								return component.get_id();
							},
							data_);
					}

					[[nodiscard]]
					Bound get_bounds() const {
						return std::visit(
							[](const auto &component) -> Bound {
								using Bounds = std::remove_cvref_t<decltype(component.get_bounds())>;
								return Bound{std::in_place_type<Bounds>, component.get_bounds()};
							},
							data_);
					}

					[[nodiscard]]
					Bound get_current_bounds() const {
						return std::visit(
							[](const auto &component) -> Bound {
								if constexpr (requires { component.get_current_bounds(); }) {
									using Bounds = std::remove_cvref_t<decltype(component.get_current_bounds())>;
									return Bound{std::in_place_type<Bounds>, component.get_current_bounds()};
								} else {
									using Bounds = std::remove_cvref_t<decltype(component.get_bounds())>;
									return Bound{std::in_place_type<Bounds>, component.get_bounds()};
								}
							},
							data_);
					}

					[[nodiscard]] Geometry::Rectangle get_index_bounds() const {
						return std::visit(
							[](const auto &component) -> Geometry::Rectangle {
								using Bounds = std::remove_cvref_t<decltype(component.get_bounds())>;
								if constexpr (std::same_as<Bounds, Geometry::Circle>) {
									const auto circle = component.get_bounds();
									const double diameter = std::max(1e-6, static_cast<double>(circle.radius.value()) * 2.0);
									return {circle.position, Geometry::Width{diameter}, Geometry::Height{diameter}};
								} else {
									return component.get_bounds();
								}
							},
							data_);
					}

					[[nodiscard]]
					std::type_index get_bound_type() const {
						return std::visit(
							[](const auto &component) {
								return std::type_index(typeid(std::remove_cvref_t<decltype(component.get_bounds())>));
							},
							data_);
					}

					[[nodiscard]]
					const AllComponents &get_data() const noexcept {
						return data_;
					}
			};
		} // namespace Components
	} // namespace Gui
} // namespace KarkinolutionView
