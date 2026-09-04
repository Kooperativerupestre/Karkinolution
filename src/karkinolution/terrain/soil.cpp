#include "karkinolution/math/geometry/models.hpp"
#include "karkinolution/math/physic/vec/model.hpp"

#include <karkinolution/core/id_generator.hpp>
#include <karkinolution/terrain/soil.hpp>

// Blueprint
Components Blueprint::gen_components() const {
	Components comps;

	for (const auto &f_component : default_components) {
		comps.add(f_component());
	}
	return comps;
}

// SoilF

SoilPiece
SoilF::gen_soil_piece(SoilTypes s_t, const GeometryForms::Radius &radius, const Vec3 &position) {

	SoilPiece soil{.radius = radius};

	soil.position = position;
	soil.type     = s_t;
	soil.id       = gen_id();

	const auto &blueprint = blueprints[s_t];


	soil.properties            = blueprint.properties;
	soil.components            = blueprint.gen_components();
	soil.required_capabilities = blueprint.required_properties;
	return soil;
}