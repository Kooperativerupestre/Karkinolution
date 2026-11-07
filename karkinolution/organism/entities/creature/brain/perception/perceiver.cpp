#include "karkinolution/organism/entities/entities.hpp"
#include "karkinolution/organism/entities/identity.hpp"
#include "karkinolution/organism/nature/grass/grass.hpp"
#include "karkinolution/organism/nature/identity.hpp"
#include "karkinolution/organism/nature/natures.hpp"
#include "karkinolution/terrain/terrain.hpp"
#include "shared/core/error.hpp"

#include <karkinolution/organism/entities/corpse/corpse.hpp>
#include <karkinolution/organism/entities/creature/brain/perception/perceiver.hpp>
#include <karkinolution/organism/entities/creature/brain/perception/perception.hpp>
#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/terrain/soil/soil.hpp>
#include <karkinolution/world/world.hpp>
#include <shared/functional/data_structures.hpp>

namespace Karkinolution::Organisms::Entities::Creature::Brain::Perception {
	PerceivedCreature Perceiver::perceive(const Creature &target, const Creature &perceiver) {
		PerceivedCreature perceived = {.id       = target.build_id(),
									   .energy   = target.body.metabolism.energy,
									   .life     = target.body.vital.life,
									   .gender   = target.ontology.gender,
									   .position = target.position,
									   .size     = target.body.morphology.size};
		return perceived;
	}

	PerceivedCorpse Perceiver::perceive(const Karkinolution::Organisms::Entities::Corpse::Corpse &target,
										const Creature                                           &perceiver) {
		PerceivedCorpse perceived = {.id       = target.build_id(),
									 .meat     = target.raw_meat,
									 .position = target.position,
									 .size     = target.size};
		return perceived;
	}

	PerceivedGrass Perceiver::perceive(const Karkinolution::Organisms::Nature::Grass::Grass &grass,
									   const Creature                                       &creature) {
		PerceivedGrass perceived = {.id       = grass.build_id(),
									.matter   = grass.matter,
									.position = grass.position,
									.radius   = grass.radius};
		return perceived;
	}

	PerceivedSoil Perceiver::perceive(const Karkinolution::Terrain::Soil::SoilPiece &soil, const Creature &perceiver) {
		const auto* damage = soil.components.try_get<Karkinolution::Terrain::Soil::SoilPieceComponents::Damage>();
		const auto* movement_cost =
			soil.components.try_get<Karkinolution::Terrain::Soil::SoilPieceComponents::MovementCost>();

		return PerceivedSoil{.id            = soil.id,
							 .position      = soil.position,
							 .radius        = soil.radius,
							 .damage        = damage != nullptr ? std::optional{*damage} : std::nullopt,
							 .movement_cost = movement_cost != nullptr ? std::optional{*movement_cost} : std::nullopt};
	}

	Perception Perceiver::perceive(const Creature &perceiver, const Karkinolution::World::World &world) {
		PerceptionEntityRegistry p_entities;
		PerceptionSoilRegistry   p_soils;
		PerceptionNatureRegistry p_natures;
		PerceptionData           data;

		const auto &radius = perceiver.genome.core_genome.vision_radius;

		const auto soils_id = world.see_territory().find(radius, perceiver.position);
		const auto entities_id =
			world.see_entity_map().find(KShared::Math::Units::Size{radius.value, radius.value}, perceiver.position);
		const auto natures_id = world.see_natures().find(radius, perceiver.position);

		p_entities.reserve(entities_id.size());
		p_soils.reserve(soils_id.size());
		p_natures.reserve(natures_id.size());

		for (auto id : soils_id) {
			const auto &soil = world.see_territory().registry().at(id);
			(void)KShared::Functional::try_add(p_soils, id, perceive(soil, perceiver));
			data.insert(id, Karkinolution::Terrain::Trees::RTree::BoxConversion::to_box(soil));
		}

		for (auto id : entities_id) {
			if (id.type == EntityTypes::EGG) {
				throw SimulationError("It was not implemented the perceive of egg");
			} else if (id.type == EntityTypes::EMBRYO) {
				throw SimulationError("It was not implemented the perceive of embryo");
			}
			const auto &entity = world.see_entities_registry().at(id);
			if (id.type == EntityTypes::CREATURE) {

				(void)KShared::Functional::try_add(p_entities,
												   id,
												   PerceivedEntity{perceive(std::get<Creature>(entity), perceiver)});
			} else { // == Corpse
				(void)KShared::Functional::try_add(
					p_entities,
					id,
					PerceivedEntity{
						perceive(std::get<Karkinolution::Organisms::Entities::Corpse::Corpse>(entity), perceiver)});
			}
			data.insert(id,
						Karkinolution::Terrain::Trees::RTree::BoxConversion::to_box(
							Karkinolution::Organisms::Entities::EntityGetters::get_size(entity),
							Karkinolution::Organisms::Entities::EntityGetters::get_position(entity)));
		}

		for (auto id : natures_id) {
			const auto &nature = world.see_natures().registry().at(id);

			if (id.type == Karkinolution::Organisms::Nature::NatureTypes::GRASS) {
				(void)KShared::Functional::try_add(
					p_natures,
					id,
					PerceivedNature{
						perceive(std::get<Karkinolution::Organisms::Nature::Grass::Grass>(nature), perceiver)});
			}
			data.insert(id,
						Karkinolution::Terrain::Trees::RTree::BoxConversion::to_box(
							Karkinolution::Organisms::Nature::NatureGetters::get_radius(nature),
							Karkinolution::Organisms::Nature::NatureGetters::get_position(nature)));
		}

		Vec2 farthest;

		farthest.x = std::min(radius.value + perceiver.position.x, world.see_territory().size().width.value);
		farthest.y = std::min(radius.value + perceiver.position.y, world.see_territory().size().height.value);
		return Perception{data, radius, p_entities, p_soils, p_natures, perceiver.position, farthest};
	}
} // namespace Karkinolution::Organisms::Entities::Creature::Brain::Perception
