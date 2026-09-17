#include "karkinolution/world/world.hpp"

#include "karkinolution/organism/registry.hpp"

#include <karkinolution/terrain/terrain_gen.hpp>

World WorldFactory::create_world(const WorldPreset &preset) {
	return World{
		.entity_map        = EntityMap{},
		.organism_registry = OrganismRegistry{},
		.natures           = NaturesMap{preset.size},

		.territory = TerrainFactory::gen_terrain(preset.size,
												 preset.seed,
												 TerrainScale::NORMAL,
												 preset.epsilon,
												 GeometryForms::Radius(10.0),
												 GeometryForms::Radius(20.0)),
	};
}