#pragma once

#include "karkinolution/organism/registry.hpp"
#include "karkinolution/terrain/natures_map.hpp"

#include <karkinolution/organism/registry.hpp>
#include <karkinolution/terrain/entity_map.hpp>
#include <karkinolution/terrain/natures_map.hpp>
#include <karkinolution/terrain/terrain.hpp>

struct World {
		EntityMap        entity_map;
		OrganismRegistry organism_registry;
		NaturesMap       natures;
		Territory        territory;
};