#include "opsiamethystos/graphics/panel/components/low_level/circle.hpp"

#include "opsiamethystos/math/geometry/geometry.hpp"

#include <algorithm>
#include <opsiamethystos/graphics/render/render.hpp>

namespace Opsiamethystos::Graphics::Panel::Components {

	LowLevel::Circle::Circle(const Opsiamethystos::Math::Geometry::Circle &bounds, const KShared::Colors::Color &color)
		: bounds_(bounds)
		, color(color) {}

	bool LowLevel::Circle::set_slot(const Opsiamethystos::Math::Geometry::Rectangle &slot) {
		// Using the smaller extent produces the centered inscribed circle required by
		// the rectangular layout contract and avoids stretching the circle into an oval.
		const float diameter =
			std::min(static_cast<float>(slot.width.value()), static_cast<float>(slot.height.value()));
		if (diameter <= 0.0F) {
			return false;
		}

		const Math::Geometry::Circle candidate{slot.position, Math::Geometry::Radius{diameter / 2.0F}};
		bounds_ = candidate;
		return true;
	}

	void LowLevel::Circle::draw() const {
		draw({0, 0});
	}

	void LowLevel::Circle::draw(const Math::Geometry::Position &origin) const {
		Render::draw_circle(Math::Geometry::Circle{Math::Geometry::Position{bounds_.position.x + origin.x,
																			bounds_.position.y + origin.y},
												   bounds_.radius},
							color);
	}

	void LowLevel::Circle::draw_bounds(Opsiamethystos::Math::Geometry::Thickness thickness) const {
		Render::draw_ring(bounds_, thickness, color);
	}

} // namespace Opsiamethystos::Graphics::Panel::Components