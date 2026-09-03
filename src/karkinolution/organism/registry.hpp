#pragma once
#include "karkinolution/core/error.hpp"

#include <karkinolution/core/basestorage.hpp>
#include <karkinolution/organism/entities/entities.hpp>
#include <karkinolution/organism/pathogens/registry.hpp>

struct OrganismRegistry {
		EntitiesRegistry entities;
		PathogenRegistry pathogens;
};
