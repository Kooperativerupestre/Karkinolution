#include "karkinolution/organism/entities/corpse/physiology.hpp"
#include "karkinolution/organism/entities/creature/constructor.hpp"
#include "karkinolution/organism/entities/creature/physiology.hpp"
#include "karkinolution/organism/entities/embryo/physiology.hpp"
#include "karkinolution/organism/registry.hpp"
#include "shared/core/id.hpp"
#include "shared/math/physic/vec/model.hpp"

#include <gtest/gtest.h>

using Karkinolution::Organisms::Energy;
using Karkinolution::Organisms::Health;
using Karkinolution::Organisms::Life;
using Karkinolution::Organisms::OrganismRegistry;
using Karkinolution::Organisms::Entities::Corpse::Corpse;
using KShared::Core::Id::gen_id;
using KShared::Math::Area;
using KShared::Math::LV::CT::NormalizedValue;
using KShared::Math::Vec::Vec2;
using CreatureEntity = Karkinolution::Organisms::Entities::Creature::Creature;
using Karkinolution::Organisms::Entities::Creature::CreatureSpecies;
using Karkinolution::Organisms::Entities::Creature::Gender;
using Karkinolution::Organisms::Entities::Creature::Constructor::CreatureBlueprint;
namespace CreatureConstructor = Karkinolution::Organisms::Entities::Creature::Constructor::CreatureConstructor;
using Karkinolution::Organisms::Entities::Embryo::Embryo;

namespace EntityIDF                 = Karkinolution::Organisms::Entities;
namespace CreaturePhysiology        = Karkinolution::Organisms::Entities::Creature::Physiology;
namespace CreatureGrowingPhysiology = Karkinolution::Organisms::Entities::Creature::Physiology::Growing;
namespace CorpsePhysiology          = Karkinolution::Organisms::Entities::Corpse::Physiology;
namespace EmbryoPhysiology          = Karkinolution::Organisms::Entities::Embryo::Physiology::EmbryoPhysiology;
namespace EmbryoGrowingPhysiology   = Karkinolution::Organisms::Entities::Embryo::Physiology::EmbryoGrowingPhysiology;

using Karkinolution::Organisms::Entities::Embryo::Physiology::BASE_VALUE_LIFE_INCREMENT;

// --- CreaturePhysiology ---

TEST(CreaturePhysiology, IsDeadIdempotency) {
	const auto id    = gen_id();
	auto       creat = CreatureConstructor::from_blueprint(CreatureBlueprint{.specie   = CreatureSpecies::CRAB,
																			 .id       = id,
																			 .name     = "A3",
																			 .gender   = Gender::FEMALE,
																			 .position = Vec2{0.0, 0.0}});

	OrganismRegistry registry;
	registry.entities_registry.add(std::move(creat));
	auto &creature = registry.entities_registry.at_creature(EntityIDF::create_creature_id(id));

	const bool dead_1 = CreaturePhysiology::is_dead(creature);
	const bool dead_2 = CreaturePhysiology::is_dead(creature);
	ASSERT_EQ(dead_1, dead_2);
	ASSERT_FALSE(dead_1);

	creature.body.vital.life.zero();
	const bool dead_zero_life_1 = CreaturePhysiology::is_dead(creature);
	const bool dead_zero_life_2 = CreaturePhysiology::is_dead(creature);
	ASSERT_EQ(dead_zero_life_1, dead_zero_life_2);
	ASSERT_TRUE(dead_zero_life_1);

	creature.body.vital.life.full();
	creature.body.metabolism.energy.zero();
	const bool dead_zero_energy_1 = CreaturePhysiology::is_dead(creature);
	const bool dead_zero_energy_2 = CreaturePhysiology::is_dead(creature);
	ASSERT_EQ(dead_zero_energy_1, dead_zero_energy_2);
	ASSERT_TRUE(dead_zero_energy_1);
}

TEST(CreaturePhysiology, HatchEmbryoIdempotency) {
	const auto id    = gen_id();
	auto       creat = CreatureConstructor::from_blueprint(CreatureBlueprint{.specie   = CreatureSpecies::CRAB,
																			 .id       = id,
																			 .name     = "A3",
																			 .gender   = Gender::FEMALE,
																			 .position = Vec2{0.0, 0.0}});

	const Embryo embryo{.id       = 42,
						.energy   = Energy{50.0f, 100.0f},
						.life     = Life{80.0f, 100.0f},
						.health   = Health{0.9f},
						.area     = Area{0.5f},
						.genome   = creat.genome,
						.age      = 5,
						.position = Vec2{1.0f, 2.0f}};

	const float initial_energy = embryo.energy.value();
	const float initial_life   = embryo.life.value();
	const float initial_health = embryo.health.value();
	const float initial_volume = embryo.area.value;

	CreatureEntity neonate_1 = CreaturePhysiology::hatch_embryo(embryo);
	CreatureEntity neonate_2 = CreaturePhysiology::hatch_embryo(embryo);

	ASSERT_FLOAT_EQ(embryo.energy.value(), initial_energy);
	ASSERT_FLOAT_EQ(embryo.life.value(), initial_life);
	ASSERT_FLOAT_EQ(embryo.health.value(), initial_health);
	ASSERT_FLOAT_EQ(embryo.area.value, initial_volume);

	ASSERT_FLOAT_EQ(neonate_1.body.metabolism.energy.value(), neonate_2.body.metabolism.energy.value());
	ASSERT_FLOAT_EQ(neonate_1.body.vital.life.value(), neonate_2.body.vital.life.value());
	ASSERT_FLOAT_EQ(neonate_1.body.vital.health.value(), neonate_2.body.vital.health.value());
	ASSERT_FLOAT_EQ(neonate_1.position.x, neonate_2.position.x);
	ASSERT_FLOAT_EQ(neonate_1.position.y, neonate_2.position.y);
	ASSERT_EQ(neonate_1.ontology.id, neonate_2.ontology.id);
}

// --- CreatureGrowingPhysiology ---

TEST(CreatureGrowingPhysiology, Idempotency) {
	const auto id    = gen_id();
	auto       creat = CreatureConstructor::from_blueprint(CreatureBlueprint{.specie   = CreatureSpecies::CRAB,
																			 .id       = id,
																			 .name     = "A3",
																			 .gender   = Gender::FEMALE,
																			 .position = Vec2{0.0, 0.0}});

	OrganismRegistry registry;
	registry.entities_registry.add(std::move(creat));
	const auto &creature = registry.entities_registry.at_creature(EntityIDF::create_creature_id(id));

	const auto lateral_increment_1 =
		CreatureGrowingPhysiology::get_new_width_increment(creature, registry.entities_registry);
	const auto lateral_increment_2 =
		CreatureGrowingPhysiology::get_new_width_increment(creature, registry.entities_registry);
	ASSERT_FLOAT_EQ(lateral_increment_1, lateral_increment_2);

	const auto height_increment_1 =
		CreatureGrowingPhysiology::get_new_height_increment(creature, registry.entities_registry);
	const auto height_increment_2 =
		CreatureGrowingPhysiology::get_new_height_increment(creature, registry.entities_registry);
	ASSERT_FLOAT_EQ(height_increment_1, height_increment_2);

	const auto max_energy_1 =
		CreatureGrowingPhysiology::get_new_max_energy_increment(creature, registry.entities_registry);
	const auto max_energy_2 =
		CreatureGrowingPhysiology::get_new_max_energy_increment(creature, registry.entities_registry);
	ASSERT_FLOAT_EQ(max_energy_1.gain, max_energy_2.gain);
	ASSERT_FLOAT_EQ(max_energy_1.cost.reserved_energy, max_energy_2.cost.reserved_energy);

	const auto max_reserved_1 =
		CreatureGrowingPhysiology::get_new_max_energy_reserved_increment(creature, registry.entities_registry);
	const auto max_reserved_2 =
		CreatureGrowingPhysiology::get_new_max_energy_reserved_increment(creature, registry.entities_registry);
	ASSERT_FLOAT_EQ(max_reserved_1.gain, max_reserved_2.gain);
	ASSERT_FLOAT_EQ(max_reserved_1.cost.reserved_energy, max_reserved_2.cost.reserved_energy);

	const auto max_life_1 = CreatureGrowingPhysiology::get_new_max_life_increment(creature, registry.entities_registry);
	const auto max_life_2 = CreatureGrowingPhysiology::get_new_max_life_increment(creature, registry.entities_registry);
	ASSERT_FLOAT_EQ(max_life_1.gain, max_life_2.gain);
	ASSERT_FLOAT_EQ(max_life_1.cost.reserved_energy, max_life_2.cost.reserved_energy);
}

// --- CorpsePhysiology ---

TEST(CorpsePhysiology, GenerateCorpseFromCreatureIdempotency) {
	const auto id    = gen_id();
	auto       creat = CreatureConstructor::from_blueprint(CreatureBlueprint{.specie   = CreatureSpecies::CRAB,
																			 .id       = id,
																			 .name     = "A3",
																			 .gender   = Gender::FEMALE,
																			 .position = Vec2{1.0f, 2.0f}});

	OrganismRegistry registry;
	registry.entities_registry.add(std::move(creat));
	const auto &creature = registry.entities_registry.at_creature(EntityIDF::create_creature_id(id));

	const float initial_energy = creature.body.metabolism.energy.value();
	const float initial_life   = creature.body.vital.life.value();

	Corpse corpse_1 = CorpsePhysiology::generate_corpse(creature);
	Corpse corpse_2 = CorpsePhysiology::generate_corpse(creature);

	ASSERT_FLOAT_EQ(creature.body.metabolism.energy.value(), initial_energy);
	ASSERT_FLOAT_EQ(creature.body.vital.life.value(), initial_life);

	ASSERT_FLOAT_EQ(corpse_1.raw_meat.value, corpse_2.raw_meat.value);
	ASSERT_FLOAT_EQ(corpse_1.position.x, corpse_2.position.x);
	ASSERT_FLOAT_EQ(corpse_1.position.y, corpse_2.position.y);
	ASSERT_FLOAT_EQ(corpse_1.size.width.value, corpse_2.size.width.value);
	ASSERT_FLOAT_EQ(corpse_1.size.height.value, corpse_2.size.height.value);
}

TEST(CorpsePhysiology, GenerateCorpseFromEmbryoIdempotency) {
	const auto id    = gen_id();
	auto       creat = CreatureConstructor::from_blueprint(CreatureBlueprint{.specie   = CreatureSpecies::CRAB,
																			 .id       = id,
																			 .name     = "A3",
																			 .gender   = Gender::FEMALE,
																			 .position = Vec2{0.0, 0.0}});

	const Embryo embryo{.id       = 42,
						.energy   = Energy{50.0f, 100.0f},
						.life     = Life{80.0f, 100.0f},
						.health   = Health{0.9f},
						.area     = Area{0.5f},
						.genome   = creat.genome,
						.age      = 5,
						.position = Vec2{4.0f, 5.0f}};

	const float initial_energy = embryo.energy.value();
	const float initial_life   = embryo.life.value();

	Corpse corpse_1 = CorpsePhysiology::generate_corpse(embryo);
	Corpse corpse_2 = CorpsePhysiology::generate_corpse(embryo);

	ASSERT_FLOAT_EQ(embryo.energy.value(), initial_energy);
	ASSERT_FLOAT_EQ(embryo.life.value(), initial_life);

	ASSERT_FLOAT_EQ(corpse_1.raw_meat.value, corpse_2.raw_meat.value);
	ASSERT_FLOAT_EQ(corpse_1.position.x, corpse_2.position.x);
	ASSERT_FLOAT_EQ(corpse_1.position.y, corpse_2.position.y);
	ASSERT_FLOAT_EQ(corpse_1.size.width.value, corpse_2.size.width.value);
	ASSERT_FLOAT_EQ(corpse_1.size.height.value, corpse_2.size.height.value);
}

// --- EmbryoPhysiology ---

TEST(EmbryoPhysiology, IsDeadIdempotency) {
	const auto id    = gen_id();
	auto       creat = CreatureConstructor::from_blueprint(CreatureBlueprint{.specie   = CreatureSpecies::CRAB,
																			 .id       = id,
																			 .name     = "A3",
																			 .gender   = Gender::FEMALE,
																			 .position = Vec2{0.0, 0.0}});

	Embryo embryo{.id       = 42,
				  .energy   = Energy{50.0f, 100.0f},
				  .life     = Life{80.0f, 100.0f},
				  .health   = Health{0.9f},
				  .area     = Area{0.5f},
				  .genome   = creat.genome,
				  .age      = 5,
				  .position = Vec2{1.0f, 2.0f}};

	const bool dead_1 = EmbryoPhysiology::is_dead(embryo);
	const bool dead_2 = EmbryoPhysiology::is_dead(embryo);
	ASSERT_EQ(dead_1, dead_2);
	ASSERT_FALSE(dead_1);

	embryo.life.zero();
	const bool dead_life_1 = EmbryoPhysiology::is_dead(embryo);
	const bool dead_life_2 = EmbryoPhysiology::is_dead(embryo);
	ASSERT_EQ(dead_life_1, dead_life_2);
	ASSERT_TRUE(dead_life_1);

	embryo.life.full();
	embryo.energy.zero();
	const bool dead_energy_1 = EmbryoPhysiology::is_dead(embryo);
	const bool dead_energy_2 = EmbryoPhysiology::is_dead(embryo);
	ASSERT_EQ(dead_energy_1, dead_energy_2);
	ASSERT_TRUE(dead_energy_1);
}

TEST(EmbryoPhysiology, BasalMetabolismIdempotency) {
	const auto id    = gen_id();
	auto       creat = CreatureConstructor::from_blueprint(CreatureBlueprint{.specie   = CreatureSpecies::CRAB,
																			 .id       = id,
																			 .name     = "A3",
																			 .gender   = Gender::FEMALE,
																			 .position = Vec2{0.0, 0.0}});

	const Embryo embryo{.id       = 42,
						.energy   = Energy{50.0f, 100.0f},
						.life     = Life{80.0f, 100.0f},
						.health   = Health{0.9f},
						.area     = Area{0.5f},
						.genome   = creat.genome,
						.age      = 5,
						.position = Vec2{1.0f, 2.0f}};

	const float meta_1 = EmbryoPhysiology::basal_metabolism(embryo);
	const float meta_2 = EmbryoPhysiology::basal_metabolism(embryo);
	ASSERT_FLOAT_EQ(meta_1, meta_2);
}

// --- EmbryoGrowingPhysiology ---

TEST(EmbryoGrowingPhysiology, Idempotency) {
	const auto id    = gen_id();
	auto       creat = CreatureConstructor::from_blueprint(CreatureBlueprint{.specie   = CreatureSpecies::CRAB,
																			 .id       = id,
																			 .name     = "A3",
																			 .gender   = Gender::FEMALE,
																			 .position = Vec2{0.0, 0.0}});

	const Embryo embryo{.id       = 42,
						.energy   = Energy{50.0f, 100.0f},
						.life     = Life{80.0f, 100.0f},
						.health   = Health{0.9f},
						.area     = Area{0.5f},
						.genome   = creat.genome,
						.age      = 5,
						.position = Vec2{1.0f, 2.0f}};

	const auto inc_1 = EmbryoGrowingPhysiology::get_increment(NormalizedValue<float>{0.4f}, BASE_VALUE_LIFE_INCREMENT);
	const auto inc_2 = EmbryoGrowingPhysiology::get_increment(NormalizedValue<float>{0.4f}, BASE_VALUE_LIFE_INCREMENT);
	ASSERT_FLOAT_EQ(inc_1.value(), inc_2.value());

	const auto max_energy_1 = EmbryoGrowingPhysiology::get_new_max_energy_increment(embryo);
	const auto max_energy_2 = EmbryoGrowingPhysiology::get_new_max_energy_increment(embryo);
	ASSERT_FLOAT_EQ(max_energy_1.cost.value(), max_energy_2.cost.value());
	ASSERT_FLOAT_EQ(max_energy_1.gain, max_energy_2.gain);

	const auto max_life_1 = EmbryoGrowingPhysiology::get_new_max_life_increment(embryo);
	const auto max_life_2 = EmbryoGrowingPhysiology::get_new_max_life_increment(embryo);
	ASSERT_FLOAT_EQ(max_life_1.cost.value(), max_life_2.cost.value());
	ASSERT_FLOAT_EQ(max_life_1.gain, max_life_2.gain);

	const auto life_1 = EmbryoGrowingPhysiology::get_new_life_increment(embryo);
	const auto life_2 = EmbryoGrowingPhysiology::get_new_life_increment(embryo);
	ASSERT_FLOAT_EQ(life_1.cost.value(), life_2.cost.value());
	ASSERT_FLOAT_EQ(life_1.gain, life_2.gain);

	const auto energy_1 = EmbryoGrowingPhysiology::get_new_energy_increment(embryo);
	const auto energy_2 = EmbryoGrowingPhysiology::get_new_energy_increment(embryo);
	ASSERT_FLOAT_EQ(energy_1.cost.value(), energy_2.cost.value());
	ASSERT_FLOAT_EQ(energy_1.gain, energy_2.gain);

	const auto health_1 = EmbryoGrowingPhysiology::get_new_health_increment(embryo);
	const auto health_2 = EmbryoGrowingPhysiology::get_new_health_increment(embryo);
	ASSERT_FLOAT_EQ(health_1.cost.value(), health_2.cost.value());
	ASSERT_FLOAT_EQ(health_1.gain, health_2.gain);

	const auto volume_1 = EmbryoGrowingPhysiology::get_new_area_increment(embryo);
	const auto volume_2 = EmbryoGrowingPhysiology::get_new_area_increment(embryo);
	ASSERT_FLOAT_EQ(volume_1.cost.value(), volume_2.cost.value());
	ASSERT_FLOAT_EQ(volume_1.gain, volume_2.gain);
}
