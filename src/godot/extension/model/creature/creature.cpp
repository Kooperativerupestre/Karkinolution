#include "model/creature/creature.hpp"

#include <karkinolution/binary/deserialization/interpreters/models.hpp>
#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/organism/entities/creature/ontology.hpp>
#include <karkinolution/organism/entities/genetics/genetic.hpp>

void GodotCreature::_bind_methods() {
	godot::ClassDB::bind_method(godot::D_METHOD("get_id"), &GodotCreature::get_id);
	godot::ClassDB::bind_method(godot::D_METHOD("set_id", "id"), &GodotCreature::set_id);
	godot::ClassDB::add_property("GodotCreature",
								 godot::PropertyInfo(godot::Variant::INT, "id"),
								 "set_id",
								 "get_id");

	godot::ClassDB::bind_method(godot::D_METHOD("get_gender"), &GodotCreature::get_gender);
	godot::ClassDB::bind_method(godot::D_METHOD("set_gender", "gender"),
								&GodotCreature::set_gender);
	godot::ClassDB::add_property("GodotCreature",
								 godot::PropertyInfo(godot::Variant::INT, "gender"),
								 "set_gender",
								 "get_gender");

	godot::ClassDB::bind_method(godot::D_METHOD("get_specie"), &GodotCreature::get_specie);
	godot::ClassDB::bind_method(godot::D_METHOD("set_specie", "specie"),
								&GodotCreature::set_specie);
	godot::ClassDB::add_property("GodotCreature",
								 godot::PropertyInfo(godot::Variant::INT, "specie"),
								 "set_specie",
								 "get_specie");

	godot::ClassDB::bind_method(godot::D_METHOD("get_gender_name"),
								&GodotCreature::get_gender_name);
	godot::ClassDB::bind_method(godot::D_METHOD("get_specie_name"),
								&GodotCreature::get_specie_name);
}

GodotCreature::GodotCreature()
	: id_(0)
	, gender_(0)
	, specie_(0) {}

GodotCreature::GodotCreature(std::uint64_t id, std::uint8_t gender, std::uint8_t specie)
	: id_(id)
	, gender_(gender)
	, specie_(specie) {}

std::uint64_t GodotCreature::get_id() const {
	return id_;
}

void GodotCreature::set_id(std::uint64_t id) {
	id_ = id;
}

std::uint8_t GodotCreature::get_gender() const {
	return gender_;
}

void GodotCreature::set_gender(std::uint8_t gender) {
	gender_ = gender;
}

std::uint8_t GodotCreature::get_specie() const {
	return specie_;
}

void GodotCreature::set_specie(std::uint8_t specie) {
	specie_ = specie;
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
	creature->set_id(id);
	creature->set_gender(static_cast<std::uint8_t>(deserialized.gender));
	creature->set_specie(static_cast<std::uint8_t>(deserialized.specie));
	return creature;
}

godot::Ref<GodotCreature> GodotCreature::from_core(const ::Creature &creature) {
	godot::Ref<GodotCreature> result;
	result.instantiate();
	result->set_id(creature.ontology.id);
	result->set_gender(static_cast<std::uint8_t>(creature.ontology.gender));
	result->set_specie(static_cast<std::uint8_t>(creature.genome.core_genome.specie));
	return result;
}
