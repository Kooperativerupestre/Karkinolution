#include "godot/extension/model/creature.hpp"

#include <godot/godot_string.hpp>
#include <karkinolution/organism/entities/creature/ontology.hpp>
#include <karkinolution/organism/entities/genetics/genetic.hpp>
#include <karkinolution/utils/enums_translator.hpp>

using KarkinolutionGodot::Models::Organisms::Creature::GodotCreature;

void GodotCreature::_bind_methods() {
	godot::ClassDB::bind_method(godot::D_METHOD("get_id"), &GodotCreature::get_id);
	godot::ClassDB::bind_method(godot::D_METHOD("get_gender"), &GodotCreature::get_gender);
	godot::ClassDB::bind_method(godot::D_METHOD("get_specie"), &GodotCreature::get_specie);
	godot::ClassDB::bind_method(godot::D_METHOD("get_position"), &GodotCreature::get_position);
	godot::ClassDB::bind_method(godot::D_METHOD("get_name"), &GodotCreature::get_name);
	godot::ClassDB::bind_method(godot::D_METHOD("get_gender_name"),
								&GodotCreature::get_gender_name);
	godot::ClassDB::bind_method(godot::D_METHOD("get_specie_name"),
								&GodotCreature::get_specie_name);
}

GodotCreature::GodotCreature() {}

GodotCreature::GodotCreature(std::int64_t          id,
							 std::uint8_t          gender,
							 std::uint8_t          specie,
							 const godot::Vector3 &position,
							 const godot::String  &name)
	: id_(id)
	, gender_(gender)
	, specie_(specie)
	, position_(position)
	, name_(name) {}

std::int64_t GodotCreature::get_id() const {
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

godot::String GodotCreature::get_name() const {
	return name_;
}

godot::String GodotCreature::get_gender_name() const {
	return to_godot_string(Karkinolution::Utils::EnumsToString::make_gender_string(gender_));
}

godot::String GodotCreature::get_specie_name() const {
	return to_godot_string(Karkinolution::Utils::EnumsToString::make_specie_string(specie_));
}
