#include "karkinolution/organism/entities/corpse/physiology.hpp"
#include "karkinolution/organism/entities/creature/constructor.hpp"
#include "karkinolution/organism/entities/creature/physiology.hpp"
#include "karkinolution/organism/entities/embryo/physiology.hpp"
#include "karkinolution/organism/registry.hpp"

#include <gtest/gtest.h>

// --- CreaturePhysiology ---

TEST(CreaturePhysiology, GetToAgeEffectsIdempotency) {
	const auto id = gen_id();
	auto creat = CreatureConstructor::from_blueprint(CreatureBlueprint{
		.specie = CreatureSpecies::CRAB,
		.id = id,
		.name = "A3",
		.gender = Gender::FEMALE,
		.position = Vec3{0.0, 0.0, 0.0}});

	OrganismRegistry registry;
	registry.entities.add(EntityIDF::create_creature_id(id), std::move(creat));
	auto &creature = registry.entities.at_creature(EntityIDF::create_creature_id(id));

	const float initial_age = creature.body.age.value;

	const auto effect_1 = CreaturePhysiology::get_to_age_effects(creature, 1);
	const auto effect_2 = CreaturePhysiology::get_to_age_effects(creature, 1);

	ASSERT_EQ(effect_1, effect_2);
	ASSERT_FLOAT_EQ(creature.body.age.value, initial_age);
}

TEST(CreaturePhysiology, GetDiseasesEffectIdempotency) {
	const auto id = gen_id();
	auto creat = CreatureConstructor::from_blueprint(CreatureBlueprint{
		.specie = CreatureSpecies::CRAB,
		.id = id,
		.name = "A3",
		.gender = Gender::FEMALE,
		.position = Vec3{0.0, 0.0, 0.0}});

	OrganismRegistry registry;
	registry.entities.add(EntityIDF::create_creature_id(id), std::move(creat));
	const auto &creature = registry.entities.at_creature(EntityIDF::create_creature_id(id));

	const auto disease_1 = CreaturePhysiology::get_diseases_effect(creature, registry.entities);
	const auto disease_2 = CreaturePhysiology::get_diseases_effect(creature, registry.entities);

	ASSERT_EQ(disease_1, disease_2);
}

TEST(CreaturePhysiology, IsDeadIdempotency) {
	const auto id = gen_id();
	auto creat = CreatureConstructor::from_blueprint(CreatureBlueprint{
		.specie = CreatureSpecies::CRAB,
		.id = id,
		.name = "A3",
		.gender = Gender::FEMALE,
		.position = Vec3{0.0, 0.0, 0.0}});

	OrganismRegistry registry;
	registry.entities.add(EntityIDF::create_creature_id(id), std::move(creat));
	auto &creature = registry.entities.at_creature(EntityIDF::create_creature_id(id));

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
	const auto id = gen_id();
	auto creat = CreatureConstructor::from_blueprint(CreatureBlueprint{
		.specie = CreatureSpecies::CRAB,
		.id = id,
		.name = "A3",
		.gender = Gender::FEMALE,
		.position = Vec3{0.0, 0.0, 0.0}});

	const Embryo embryo{
		.id = 42,
		.energy = Energy{50.0f, 100.0f},
		.life = Life{80.0f, 100.0f},
		.health = Health{0.9f},
		.volume = Volume{0.5f},
		.genome = creat.genome,
		.age = 5,
		.position = Vec3{1.0f, 2.0f, 3.0f}};

	const float initial_energy = embryo.energy.value();
	const float initial_life = embryo.life.value();
	const float initial_health = embryo.health.value();
	const float initial_volume = embryo.volume.value;

	Creature neonate_1 = CreaturePhysiology::hatch_embryo(embryo);
	Creature neonate_2 = CreaturePhysiology::hatch_embryo(embryo);

	ASSERT_FLOAT_EQ(embryo.energy.value(), initial_energy);
	ASSERT_FLOAT_EQ(embryo.life.value(), initial_life);
	ASSERT_FLOAT_EQ(embryo.health.value(), initial_health);
	ASSERT_FLOAT_EQ(embryo.volume.value, initial_volume);

	ASSERT_FLOAT_EQ(neonate_1.body.metabolism.energy.value(), neonate_2.body.metabolism.energy.value());
	ASSERT_FLOAT_EQ(neonate_1.body.vital.life.value(), neonate_2.body.vital.life.value());
	ASSERT_FLOAT_EQ(neonate_1.body.vital.health.value(), neonate_2.body.vital.health.value());
	ASSERT_FLOAT_EQ(neonate_1.position.x, neonate_2.position.x);
	ASSERT_FLOAT_EQ(neonate_1.position.y, neonate_2.position.y);
	ASSERT_FLOAT_EQ(neonate_1.position.z, neonate_2.position.z);
	ASSERT_EQ(neonate_1.ontology.id, neonate_2.ontology.id);
}

// --- CreatureGrowingPhysiology ---

TEST(CreatureGrowingPhysiology, Idempotency) {
	const auto id = gen_id();
	auto creat = CreatureConstructor::from_blueprint(CreatureBlueprint{
		.specie = CreatureSpecies::CRAB,
		.id = id,
		.name = "A3",
		.gender = Gender::FEMALE,
		.position = Vec3{0.0, 0.0, 0.0}});

	OrganismRegistry registry;
	registry.entities.add(EntityIDF::create_creature_id(id), std::move(creat));
	const auto &creature = registry.entities.at_creature(EntityIDF::create_creature_id(id));

	const auto lateral_increment_1 =
		CreatureGrowingPhysiology::get_new_lateral_increment(creature, registry);
	const auto lateral_increment_2 =
		CreatureGrowingPhysiology::get_new_lateral_increment(creature, registry);
	ASSERT_FLOAT_EQ(lateral_increment_1, lateral_increment_2);

	const auto depth_increment_1 =
		CreatureGrowingPhysiology::get_new_depth_increment(creature, registry);
	const auto depth_increment_2 =
		CreatureGrowingPhysiology::get_new_depth_increment(creature, registry);
	ASSERT_FLOAT_EQ(depth_increment_1, depth_increment_2);

	const auto height_increment_1 =
		CreatureGrowingPhysiology::get_new_height_increment(creature, registry);
	const auto height_increment_2 =
		CreatureGrowingPhysiology::get_new_height_increment(creature, registry);
	ASSERT_FLOAT_EQ(height_increment_1, height_increment_2);

	const auto max_energy_1 =
		CreatureGrowingPhysiology::get_new_max_energy_increment(creature, registry);
	const auto max_energy_2 =
		CreatureGrowingPhysiology::get_new_max_energy_increment(creature, registry);
	ASSERT_FLOAT_EQ(max_energy_1.gain, max_energy_2.gain);
	ASSERT_FLOAT_EQ(max_energy_1.cost.reserved_energy, max_energy_2.cost.reserved_energy);

	const auto max_reserved_1 =
		CreatureGrowingPhysiology::get_new_max_energy_reserved_increment(creature, registry);
	const auto max_reserved_2 =
		CreatureGrowingPhysiology::get_new_max_energy_reserved_increment(creature, registry);
	ASSERT_FLOAT_EQ(max_reserved_1.gain, max_reserved_2.gain);
	ASSERT_FLOAT_EQ(max_reserved_1.cost.reserved_energy, max_reserved_2.cost.reserved_energy);

	const auto max_life_1 =
		CreatureGrowingPhysiology::get_new_max_life_increment(creature, registry);
	const auto max_life_2 =
		CreatureGrowingPhysiology::get_new_max_life_increment(creature, registry);
	ASSERT_FLOAT_EQ(max_life_1.gain, max_life_2.gain);
	ASSERT_FLOAT_EQ(max_life_1.cost.reserved_energy, max_life_2.cost.reserved_energy);

	const auto muscle_1 =
		CreatureGrowingPhysiology::get_new_muscle_increment(creature, registry);
	const auto muscle_2 =
		CreatureGrowingPhysiology::get_new_muscle_increment(creature, registry);
	ASSERT_FLOAT_EQ(muscle_1.gain, muscle_2.gain);
	ASSERT_FLOAT_EQ(muscle_1.cost.reserved_energy, muscle_2.cost.reserved_energy);

	const auto skeleton_1 =
		CreatureGrowingPhysiology::get_new_skeleton_increment(creature, registry);
	const auto skeleton_2 =
		CreatureGrowingPhysiology::get_new_skeleton_increment(creature, registry);
	ASSERT_FLOAT_EQ(skeleton_1.gain, skeleton_2.gain);
	ASSERT_FLOAT_EQ(skeleton_1.cost.reserved_energy, skeleton_2.cost.reserved_energy);
}

// --- CorpsePhysiology ---

TEST(CorpsePhysiology, GenerateCorpseFromCreatureIdempotency) {
	const auto id = gen_id();
	auto creat = CreatureConstructor::from_blueprint(CreatureBlueprint{
		.specie = CreatureSpecies::CRAB,
		.id = id,
		.name = "A3",
		.gender = Gender::FEMALE,
		.position = Vec3{1.0f, 2.0f, 3.0f}});

	OrganismRegistry registry;
	registry.entities.add(EntityIDF::create_creature_id(id), std::move(creat));
	const auto &creature = registry.entities.at_creature(EntityIDF::create_creature_id(id));

	const float initial_energy = creature.body.metabolism.energy.value();
	const float initial_life = creature.body.vital.life.value();

	Corpse corpse_1 = CorpsePhysiology::generate_corpse(creature);
	Corpse corpse_2 = CorpsePhysiology::generate_corpse(creature);

	ASSERT_FLOAT_EQ(creature.body.metabolism.energy.value(), initial_energy);
	ASSERT_FLOAT_EQ(creature.body.vital.life.value(), initial_life);

	ASSERT_FLOAT_EQ(corpse_1.raw_meat.value, corpse_2.raw_meat.value);
	ASSERT_FLOAT_EQ(corpse_1.position.x, corpse_2.position.x);
	ASSERT_FLOAT_EQ(corpse_1.position.y, corpse_2.position.y);
	ASSERT_FLOAT_EQ(corpse_1.position.z, corpse_2.position.z);
	ASSERT_FLOAT_EQ(corpse_1.size.lateral.value, corpse_2.size.lateral.value);
	ASSERT_FLOAT_EQ(corpse_1.size.height.value, corpse_2.size.height.value);
	ASSERT_FLOAT_EQ(corpse_1.size.back.value, corpse_2.size.back.value);
}

TEST(CorpsePhysiology, GenerateCorpseFromEmbryoIdempotency) {
	const auto id = gen_id();
	auto creat = CreatureConstructor::from_blueprint(CreatureBlueprint{
		.specie = CreatureSpecies::CRAB,
		.id = id,
		.name = "A3",
		.gender = Gender::FEMALE,
		.position = Vec3{0.0, 0.0, 0.0}});

	const Embryo embryo{
		.id = 42,
		.energy = Energy{50.0f, 100.0f},
		.life = Life{80.0f, 100.0f},
		.health = Health{0.9f},
		.volume = Volume{0.5f},
		.genome = creat.genome,
		.age = 5,
		.position = Vec3{4.0f, 5.0f, 6.0f}};

	const float initial_energy = embryo.energy.value();
	const float initial_life = embryo.life.value();

	Corpse corpse_1 = CorpsePhysiology::generate_corpse(embryo);
	Corpse corpse_2 = CorpsePhysiology::generate_corpse(embryo);

	ASSERT_FLOAT_EQ(embryo.energy.value(), initial_energy);
	ASSERT_FLOAT_EQ(embryo.life.value(), initial_life);

	ASSERT_FLOAT_EQ(corpse_1.raw_meat.value, corpse_2.raw_meat.value);
	ASSERT_FLOAT_EQ(corpse_1.position.x, corpse_2.position.x);
	ASSERT_FLOAT_EQ(corpse_1.position.y, corpse_2.position.y);
	ASSERT_FLOAT_EQ(corpse_1.position.z, corpse_2.position.z);
	ASSERT_FLOAT_EQ(corpse_1.size.lateral.value, corpse_2.size.lateral.value);
	ASSERT_FLOAT_EQ(corpse_1.size.height.value, corpse_2.size.height.value);
	ASSERT_FLOAT_EQ(corpse_1.size.back.value, corpse_2.size.back.value);
}

// --- EmbryoPhysiology ---

TEST(EmbryoPhysiology, IsDeadIdempotency) {
	const auto id = gen_id();
	auto creat = CreatureConstructor::from_blueprint(CreatureBlueprint{
		.specie = CreatureSpecies::CRAB,
		.id = id,
		.name = "A3",
		.gender = Gender::FEMALE,
		.position = Vec3{0.0, 0.0, 0.0}});

	Embryo embryo{
		.id = 42,
		.energy = Energy{50.0f, 100.0f},
		.life = Life{80.0f, 100.0f},
		.health = Health{0.9f},
		.volume = Volume{0.5f},
		.genome = creat.genome,
		.age = 5,
		.position = Vec3{1.0f, 2.0f, 3.0f}};

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
	const auto id = gen_id();
	auto creat = CreatureConstructor::from_blueprint(CreatureBlueprint{
		.specie = CreatureSpecies::CRAB,
		.id = id,
		.name = "A3",
		.gender = Gender::FEMALE,
		.position = Vec3{0.0, 0.0, 0.0}});

	const Embryo embryo{
		.id = 42,
		.energy = Energy{50.0f, 100.0f},
		.life = Life{80.0f, 100.0f},
		.health = Health{0.9f},
		.volume = Volume{0.5f},
		.genome = creat.genome,
		.age = 5,
		.position = Vec3{1.0f, 2.0f, 3.0f}};

	const float meta_1 = EmbryoPhysiology::basal_metabolism(embryo);
	const float meta_2 = EmbryoPhysiology::basal_metabolism(embryo);
	ASSERT_FLOAT_EQ(meta_1, meta_2);
}

// --- EmbryoGrowingPhysiology ---

TEST(EmbryoGrowingPhysiology, Idempotency) {
	const auto id = gen_id();
	auto creat = CreatureConstructor::from_blueprint(CreatureBlueprint{
		.specie = CreatureSpecies::CRAB,
		.id = id,
		.name = "A3",
		.gender = Gender::FEMALE,
		.position = Vec3{0.0, 0.0, 0.0}});

	const Embryo embryo{
		.id = 42,
		.energy = Energy{50.0f, 100.0f},
		.life = Life{80.0f, 100.0f},
		.health = Health{0.9f},
		.volume = Volume{0.5f},
		.genome = creat.genome,
		.age = 5,
		.position = Vec3{1.0f, 2.0f, 3.0f}};

	const auto inc_1 =
		EmbryoGrowingPhysiology::get_increment(NormalizedValue<float>{0.4f}, BASE_VALUE_LIFE_INCREMENT);
	const auto inc_2 =
		EmbryoGrowingPhysiology::get_increment(NormalizedValue<float>{0.4f}, BASE_VALUE_LIFE_INCREMENT);
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

	const auto volume_1 = EmbryoGrowingPhysiology::get_new_volume_increment(embryo);
	const auto volume_2 = EmbryoGrowingPhysiology::get_new_volume_increment(embryo);
	ASSERT_FLOAT_EQ(volume_1.cost.value(), volume_2.cost.value());
	ASSERT_FLOAT_EQ(volume_1.gain, volume_2.gain);
}
