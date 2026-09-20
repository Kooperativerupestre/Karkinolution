#pragma once

#include "karkinolution/organism/registry.hpp"
#include "karkinolution/terrain/natures_map.hpp"

#include <karkinolution/organism/registry.hpp>
#include <karkinolution/terrain/entity_map.hpp>
#include <karkinolution/terrain/natures_map.hpp>
#include <karkinolution/terrain/terrain.hpp>

struct World {
	public:

		EntityMap        entity_map;
		OrganismRegistry organism_registry;
		NaturesMap       natures;
		Territory        territory;
		int              time = 0;

		void pass_time() {
			++time;
		}
};

struct WorldPreset {
		int   seed;
		Size  size;
		float epsilon;
};

namespace WorldFactory {
	World create_world(const WorldPreset &preset);
} // namespace WorldFactory
