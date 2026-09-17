#include "karkinolution/binary/serialization/interpreters/creature.hpp"

#include "karkinolution/organism/entities/creature/ontology.hpp"
#include "karkinolution/organism/entities/genetics/genetic.hpp"


using CreatureSRI::CreatureBytes;

CreatureBytes CreatureSRI::serialize_creature(const Creature &creature) {
	CreatureBytes bytes;

	if (creature.ontology.gender == Gender::FEMALE) {
		bytes[0] = std::byte(0x01);
	} else { // gender == male
		bytes[0] = std::byte(0x2);
	}

	if (creature.genome.core_genome.specie == CreatureSpecies::CRAB) {
		bytes[1] = std::byte(0x01);
	} else if (creature.genome.core_genome.specie == CreatureSpecies::HIPPOPOTAMUS) {
		bytes[1] = std::byte(0x02);
	} else if (creature.genome.core_genome.specie == CreatureSpecies::CROCODILE) {
		bytes[1] = std::byte(0x03);
	} else { // specie == fish
		bytes[1] = std::byte(0x04);
	}
	return bytes;
}