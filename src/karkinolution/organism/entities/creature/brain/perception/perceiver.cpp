#include "karkinolution/core/error.hpp"
#include "karkinolution/organism/entities/entities.hpp"
#include "karkinolution/organism/entities/identity.hpp"
#include "karkinolution/organism/nature/grass/grass.hpp"
#include "karkinolution/organism/nature/identity.hpp"
#include "karkinolution/organism/nature/natures.hpp"
#include "karkinolution/terrain/terrain.hpp"

#include <karkinolution/organism/entities/corpse/corpse.hpp>
#include <karkinolution/organism/entities/creature/brain/perception/perceiver.hpp>
#include <karkinolution/organism/entities/creature/brain/perception/perception.hpp>
#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/terrain/soil.hpp>
#include <karkinolution/world/world.hpp>

PerceivedCreature Perceiver::perceive(const Creature &target, const Creature &perceiver) {
	PerceivedCreature perceived = {.id       = target.build_id(),
								   .energy   = target.body.metabolism.energy,
								   .life     = target.body.vital.life,
								   .gender   = target.ontology.gender,
								   .position = target.position,
								   .size     = target.body.morphology.size};
	return perceived;
}

PerceivedCorpse Perceiver::perceive(const Corpse &target, const Creature &perceiver) {
	PerceivedCorpse perceived = {.id       = target.build_id(),
								 .meat     = target.raw_meat,
								 .position = target.position,
								 .size     = target.size};
	return perceived;
}

PerceivedGrass Perceiver::perceive(const Grass &grass, const Creature &creature) {
	PerceivedGrass perceived = {.id       = grass.build_id(),
								.matter   = grass.matter,
								.position = grass.position,
								.radius   = grass.radius};
	return perceived;
}

Perception Perceiver::perceive(const Creature &perceiver, const World &world) {
	PerceptionEntityRegistry p_entities;
	PerceptionSoilRegistry   p_soils;
	PerceptionNatureRegistry p_natures;
	PerceptionData           data;

	const Radius &radius = perceiver.genome.core_genome.vision_radius;

	const auto soils_id    = world.territory.find(radius, perceiver.position);
	const auto entities_id = EntityMapMotor::find(radius, perceiver.position, world.entity_map);
	const auto natures_id  = world.natures.find(radius, perceiver.position);

	p_entities.reserve(entities_id.size());
	p_soils.reserve(soils_id.size());
	p_natures.reserve(natures_id.size());

	for (auto id : soils_id) {
		const auto &soil = world.territory.registry().at(id);
		p_soils.add(id, perceive(soil, perceiver));
		data.insert(id, BoxConversion::to_box(soil));
	}

	for (auto id : entities_id) {
		if (id.type == EntityTypes::EGG) {
			throw SimulationError("It was not implemented the perceive of egg");
		} else if (id.type == EntityTypes::EMBRYO) {
			throw SimulationError("It was not implemented the perceive of embryo");
		}
		const auto &entity = world.organism_registry.entities.at(id);
		if (id.type == EntityTypes::CREATURE) {

			p_entities.add(id, perceive(std::get<Creature>(entity), perceiver));
		} else { // == Corpse
			p_entities.add(id, perceive(std::get<Corpse>(entity), perceiver));
		}
		data.insert(id,
					BoxConversion::to_box(EntityGetters::get_size(entity),
										  EntityGetters::get_position(entity)));
	}

	for (auto id : natures_id) {
		const auto &nature = world.natures.registry().at(id);

		if (id.type == NatureTypes::GRASS) {
			p_natures.add(id, perceive(std::get<Grass>(nature), perceiver));
		}
		data.insert(id,
					BoxConversion::to_box(NatureGetters::get_radius(nature),
										  NatureGetters::get_position(nature)));
	}

	Vec3 farthest;

	farthest.z = world.territory.size().height.value;
	farthest.x =
		std::min(radius.value + perceiver.position.x, world.territory.size().lateral.value);
	farthest.y = std::min(radius.value + perceiver.position.y, world.territory.size().back.value);
	return Perception{data, radius, p_entities, p_soils, p_natures, perceiver.position, farthest};
}