#pragma once
#include "karkinolution/terrain/rtree/service.hpp"

#include <karkinolution/core/basestorage.hpp>
#include <karkinolution/organism/nature/natures.hpp>
#include <karkinolution/terrain/rtree/rtree.hpp>

class NaturesMap : public RSTService<NatureId, Nature, NatureRegistry> {
		using RSTService<NatureId, Nature, NatureRegistry>::RSTService;
};
