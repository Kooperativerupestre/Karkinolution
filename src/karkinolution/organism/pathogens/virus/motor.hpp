#pragma once
#include "karkinolution/math/stats/compile_values.hpp"

#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/organism/entities/embryo/embryo.hpp>
#include <karkinolution/organism/pathogens/virus/virus.hpp>
#include <karkinolution/organism/stats.hpp>

using OrganismStats::Health::Immunity;

namespace VirusMotor {
void apply(Creature &creature, const Vorax &virus);
void apply(Embryo &embryo, const Vorax &virus);

void apply(Embryo &embryo, const MorbusExiguus &virus);
void apply(Creature &creature, const MorbusExiguus &virus);

void transmit(Id entity_id, Id new_entity_id, VirusId virus_id);


void fight(Immunity &immunity, Vorax &virus);
void fight(Immunity &immunity, MorbusExiguus &virus);
} // namespace VirusMotor