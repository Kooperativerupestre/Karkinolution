#include "godot/extension/model/translator.hpp"

#include "godot/extension/model/aliases.hpp"
#include "godot/godot_string.hpp"

#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/vector3.hpp>

godot::Vector3 KarkinolutionGodot::Translator::translate(const Vec3 &vec) {
	return godot::Vector3{static_cast<float>(vec.x),
						  static_cast<float>(vec.y),
						  static_cast<float>(vec.z)};
}

GodotSize KarkinolutionGodot::Translator::translate(const Size &size) {
	return GodotSize{size.lateral.value, size.height.value, size.depth.value};
}

GodotCorpse KarkinolutionGodot::Translator::translate(const CorpseDSI::Models::Corpse &corpse) {
	godot::Ref<GodotSize> size;
	size.instantiate();
	size->set_value(corpse.size.lateral.value, corpse.size.height.value, corpse.size.depth.value);

	return GodotCorpse{static_cast<std::int64_t>(corpse.id),
					   corpse.raw_meat.value,
					   translate(corpse.position),
					   size};
}

GodotCreature KarkinolutionGodot::Translator::translate(std::int64_t                         id,
													   const CreatureDSI::Models::Creature &creature) {
	// TODO: Make creature carry its id
	return GodotCreature{id,
						 static_cast<std::uint8_t>(creature.gender),
						 static_cast<std::uint8_t>(creature.specie),
						 translate(creature.position),
						 to_godot_string(creature.name)};
}

GodotDamage KarkinolutionGodot::Translator::translate(const SoilDSI::Models::DamageComponent &damage) {
	return GodotDamage{damage.value};
}

GodotMovementCost
KarkinolutionGodot::Translator::translate(const SoilDSI::Models::MovementCostComponent &movement_cost) {
	return GodotMovementCost{movement_cost.cost};
}

GodotSoil KarkinolutionGodot::Translator::translate(const SoilDSI::Models::Soil &soil) {
	godot::Array properties;
	for (const auto &prop : soil.properties) {
		properties.append(static_cast<int64_t>(prop));
	}

	godot::Array required_capabilities;
	for (const auto &prop : soil.required_properties) {
		required_capabilities.append(static_cast<int64_t>(prop.raw().value));
	}

	std::optional<GodotDamage> damage;
	if (soil.damage_component.has_value()) {
		damage.emplace(translate(soil.damage_component.value()).get_value());
	}

	std::optional<GodotMovementCost> movement_cost;
	if (soil.movement_cost_component.has_value()) {
		movement_cost.emplace(translate(soil.movement_cost_component.value()).get_value());
	}

	return GodotSoil{static_cast<std::uint64_t>(soil.id),
					 static_cast<std::uint8_t>(soil.type),
					 properties,
					 required_capabilities,
					 std::move(damage),
					 std::move(movement_cost),
					 soil.radius.value,
					 translate(soil.position)};
}

GodotProperty KarkinolutionGodot::Translator::translate(const GenericProperty &property) {
	return GodotProperty{property.raw().value};
}