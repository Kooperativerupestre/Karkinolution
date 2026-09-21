#include "model/creature/creature.hpp"

#include <karkinolution/binary/deserialization/interpreters/models.hpp>
#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/organism/entities/creature/ontology.hpp>
#include <karkinolution/organism/entities/genetics/genetic.hpp>

void GodotCreature::_bind_methods() {
	godot::ClassDB::bind_method(godot::D_METHOD("get_id"), &GodotCreature::get_id);
	godot::ClassDB::bind_method(godot::D_METHOD("get_gender"), &GodotCreature::get_gender);
	godot::ClassDB::bind_method(godot::D_METHOD("get_specie"), &GodotCreature::get_specie);
	godot::ClassDB::bind_method(godot::D_METHOD("get_position"), &GodotCreature::get_position);
	godot::ClassDB::bind_method(godot::D_METHOD("get_gender_name"),
								&GodotCreature::get_gender_name);
	godot::ClassDB::bind_method(godot::D_METHOD("get_specie_name"),
								&GodotCreature::get_specie_name);
}

GodotCreature::GodotCreature()
	: id_(0)
	, gender_(0)
	, specie_(0)
	, position_(0.0, 0.0, 0.0) {}

GodotCreature::GodotCreature(std::uint64_t        id,
							 std::uint8_t         gender,
							 std::uint8_t         specie,
							 const godot::Vector3 &position)
	: id_(id)
	, gender_(gender)
	, specie_(specie)
	, position_(position) {}

std::uint64_t GodotCreature::get_id() const {
	return id_;
}

std::uint8_t GodotCreature::get_gender() const {
	return gender_;
}

std::uint8_t GodotCreature::get_specie() const {
	return specie_;
}

godot::Vector3 GodotCreature::get_position() const {
	return position_;
}

godot::String GodotCreature::get_gender_name() const {
	if (gender_ == static_cast<std::uint8_t>(Gender::FEMALE)) {
		return "FEMALE";
	}
	return "MALE";
}

godot::String GodotCreature::get_specie_name() const {
	switch (static_cast<CreatureSpecies>(specie_)) {
		case CreatureSpecies::CRAB:
			return "CRAB";
		case CreatureSpecies::FISH:
			return "FISH";
		case CreatureSpecies::CROCODILE:
			return "CROCODILE";
		case CreatureSpecies::HIPPOPOTAMUS:
			return "HIPPOPOTAMUS";
		default:
			return "UNKNOWN";
	}
}

godot::Ref<GodotCreature>
GodotCreature::from_deserialized(const DesserializedCreature &deserialized, std::uint64_t id) {
	godot::Ref<GodotCreature> creature;
	creature.instantiate();
	creature->id_       = id;
	creature->gender_   = static_cast<std::uint8_t>(deserialized.gender);
	creature->specie_   = static_cast<std::uint8_t>(deserialized.specie);
	creature->position_ = godot::Vector3(static_cast<godot::real_t>(deserialized.position.x),
										 static_cast<godot::real_t>(deserialized.position.y),
										 static_cast<godot::real_t>(deserialized.position.z));
	return creature;
}

godot::Ref<GodotCreature> GodotCreature::from_core(const ::Creature &creature) {
	godot::Ref<GodotCreature> result;
	result.instantiate();
	result->id_       = creature.ontology.id;
	result->gender_   = static_cast<std::uint8_t>(creature.ontology.gender);
	result->specie_   = static_cast<std::uint8_t>(creature.genome.core_genome.specie);
	result->position_ = godot::Vector3(static_cast<godot::real_t>(creature.position.x),
									   static_cast<godot::real_t>(creature.position.y),
									   static_cast<godot::real_t>(creature.position.z));
	return result;
}
