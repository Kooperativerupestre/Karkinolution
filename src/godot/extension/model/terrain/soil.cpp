#include "godot/extension/model/terrain/soil.hpp"

#include "karkinolution/utils/enums_translator.hpp"

#include <godot/godot_string.hpp>


using KarkinolutionGodot::Models::Soils::GodotSoilPiece;
using DamageComponent       = KarkinolutionGodot::Models::Soils::Components::GodotDamage;
using MovementCostComponent = KarkinolutionGodot::Models::Soils::Components::GodotMovementCost;

void GodotSoilPiece::_bind_methods() {
	godot::ClassDB::bind_method(godot::D_METHOD("get_id"), &GodotSoilPiece::get_id);
	godot::ClassDB::bind_method(godot::D_METHOD("get_type"), &GodotSoilPiece::get_type);
	godot::ClassDB::bind_method(godot::D_METHOD("get_type_name"), &GodotSoilPiece::get_type_name);
	godot::ClassDB::bind_method(godot::D_METHOD("get_properties"), &GodotSoilPiece::get_properties);
	godot::ClassDB::bind_method(godot::D_METHOD("get_required_capabilities"),
								&GodotSoilPiece::get_required_capabilities);
	godot::ClassDB::bind_method(godot::D_METHOD("get_radius"), &GodotSoilPiece::get_radius);
	godot::ClassDB::bind_method(godot::D_METHOD("get_position"), &GodotSoilPiece::get_position);
}

GodotSoilPiece::GodotSoilPiece()
	: position_(0.0, 0.0, 0.0) {}

GodotSoilPiece::GodotSoilPiece(std::uint64_t                        id,
							   std::uint8_t                         type,
							   const godot::Array                  &properties,
							   const godot::Array                  &required_capabilities,
							   std::optional<DamageComponent>       damage,
							   std::optional<MovementCostComponent> movement_cost,
							   double                               radius,
							   const godot::Vector3                &position)
	: id_(id)
	, type_(type)
	, properties_(properties)
	, required_capabilities_(required_capabilities)
	, radius_(radius)
	, position_(position) {}

std::uint64_t GodotSoilPiece::get_id() const {
	return id_;
}

std::uint8_t GodotSoilPiece::get_type() const {
	return type_;
}

godot::String GodotSoilPiece::get_type_name() const {
	return godot::String(
		to_godot_string(Karkinolution::Utils::EnumsToString::make_soil_type_string(type_)));
}

godot::Array GodotSoilPiece::get_properties() const {
	return properties_;
}

godot::Array GodotSoilPiece::get_required_capabilities() const {
	return required_capabilities_;
}

double GodotSoilPiece::get_radius() const {
	return radius_;
}

godot::Vector3 GodotSoilPiece::get_position() const {
	return position_;
}
