#pragma once

#include "karkinolution/organism/registry.hpp"
#include <karkinolution/organism/registry.hpp>
#include <karkinolution/terrain/entity_map.hpp>
#include <karkinolution/terrain/terrain.hpp>
struct World {
    EntityMap entity_map;
    OrganismRegistry organism_reistr;

    Territory territory;
    SoilPieceRegistry soil_registry;
};