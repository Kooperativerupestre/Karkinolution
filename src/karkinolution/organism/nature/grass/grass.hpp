#pragma once
#include "karkinolution/core/id.hpp"
#include "karkinolution/math/stats/runtime_values.hpp"

#include <karkinolution/math/geometry/models.hpp>
#include <karkinolution/math/physic/vec/model.hpp>
#include <karkinolution/math/units.hpp>
#include <karkinolution/organism/nature/identity.hpp>
#include <karkinolution/organism/stats.hpp>

class GrassMatter : public RuntimeLimitedValue<GrassMatter, float> {
		using RuntimeLimitedValue<GrassMatter, float>::RuntimeLimitedValue;
};

class Grass {
	public:

		BaseIdType id;

		PhysicsStats::Quality quality;
		Vec3                  position;
		GeometryForms::Radius radius;
		GrassMatter           matter;

		bool constexpr should_disappear() const {
			return matter.is_zero();
		}

		[[nodiscard]] NatureId constexpr build_id() const {
			return NatureId{.type = NatureTypes::GRASS, .value = id};
		}
};
