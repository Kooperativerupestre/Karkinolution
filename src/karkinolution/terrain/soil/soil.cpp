#include "shared/math/geometry/models.hpp"
#include "shared/math/physic/vec/model.hpp"

#include <shared/core/id.hpp>
#include <karkinolution/terrain/soil/soil.hpp>

namespace Karkinolution::Terrain::Soil {

	// Blueprint
	Components Blueprint::gen_components() const {
		Components comps;

		for (const auto &f_component : default_components) {
			comps.add(f_component());
		}
		return comps;
	}

	// SoilF

	SoilPiece SoilF::gen_soil_piece(SoilTypes s_t, const KShared::Math::Geometry::Radius &radius, const Vec2 &position) {

		SoilPiece soil{.radius = radius};

		soil.position = position;
		soil.type     = s_t;
		soil.id       = KShared::Core::Id::gen_id();

		const auto &blueprint = blueprints[s_t];


		soil.properties            = blueprint.properties;
		soil.components            = blueprint.gen_components();
		soil.required_capabilities = blueprint.required_properties;
		return soil;
	}

} // namespace Karkinolution::Terrain::Soil
