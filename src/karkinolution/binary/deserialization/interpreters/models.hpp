#pragma once
#include "karkinolution/organism/entities/genetics/genetic.hpp"

#include <karkinolution/organism/entities/corpse/corpse.hpp>
#include <karkinolution/organism/entities/creature/ontology.hpp>

struct DesserializedCreature {
		Gender          gender;
		CreatureSpecies specie;
		Vec3            position;
		std::string     name;
};

struct DesserializedCorpse {
		std::uint64_t id{0};
		RawMeat       raw_meat{0.0f};
		Vec3          position;
		Size          size;
};

