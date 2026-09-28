#include "godot/extension/model/terrain/soil.hpp"

void GodotSoilPiece::_bind_methods() {
	godot::ClassDB::bind_method(godot::D_METHOD("get_id"), &GodotSoilPiece::get_id);
	godot::ClassDB::bind_method(godot::D_METHOD("get_type"), &GodotSoilPiece::get_type);
	godot::ClassDB::bind_method(godot::D_METHOD("get_type_name"), &GodotSoilPiece::get_type_name);
	godot::ClassDB::bind_method(godot::D_METHOD("get_properties"), &GodotSoilPiece::get_properties);
	godot::ClassDB::bind_method(godot::D_METHOD("get_required_capabilities"),
								&GodotSoilPiece::get_required_capabilities);
	godot::ClassDB::bind_method(godot::D_METHOD("has_damage"), &GodotSoilPiece::has_damage);
	godot::ClassDB::bind_method(godot::D_METHOD("get_damage"), &GodotSoilPiece::get_damage);
	godot::ClassDB::bind_method(godot::D_METHOD("has_movement_cost"),
								&GodotSoilPiece::has_movement_cost);
	godot::ClassDB::bind_method(godot::D_METHOD("get_movement_cost"),
								&GodotSoilPiece::get_movement_cost);
	godot::ClassDB::bind_method(godot::D_METHOD("get_radius"), &GodotSoilPiece::get_radius);
	godot::ClassDB::bind_method(godot::D_METHOD("get_radius_ref"), &GodotSoilPiece::get_radius_ref);
	godot::ClassDB::bind_method(godot::D_METHOD("get_position"), &GodotSoilPiece::get_position);

	godot::ClassDB::bind_static_method("GodotSoilPiece",
									   godot::D_METHOD("create",
													   "id",
													   "type",
													   "properties",
													   "required_capabilities",
													   "has_damage",
													   "damage",
													   "has_movement_cost",
													   "movement_cost",
													   "radius",
													   "position"),
									   &GodotSoilPiece::create,
									   DEFVAL(0),
									   DEFVAL(0),
									   DEFVAL(godot::Array()),
									   DEFVAL(godot::Array()),
									   DEFVAL(false),
									   DEFVAL(0.0f),
									   DEFVAL(false),
									   DEFVAL(0.0f),
									   DEFVAL(0.0),
									   DEFVAL(godot::Vector3()));

	BIND_ENUM_CONSTANT(SAND);
	BIND_ENUM_CONSTANT(ROCK);
	BIND_ENUM_CONSTANT(DIRT);
	BIND_ENUM_CONSTANT(WATER);

	BIND_ENUM_CONSTANT(DANGEROUS);
}

GodotSoilPiece::GodotSoilPiece()
	: id_(0)
	, type_(0)
	, properties_()
	, required_capabilities_()
	, has_damage_(false)
	, damage_(0.0f)
	, has_movement_cost_(false)
	, movement_cost_(0.0f)
	, radius_(0.0)
	, position_(0.0, 0.0, 0.0) {}

GodotSoilPiece::GodotSoilPiece(std::uint64_t         id,
							   std::uint8_t          type,
							   const godot::Array   &properties,
							   const godot::Array   &required_capabilities,
							   bool                  has_damage,
							   float                 damage,
							   bool                  has_movement_cost,
							   float                 movement_cost,
							   double                radius,
							   const godot::Vector3 &position)
	: id_(id)
	, type_(type)
	, properties_(properties)
	, required_capabilities_(required_capabilities)
	, has_damage_(has_damage)
	, damage_(damage)
	, has_movement_cost_(has_movement_cost)
	, movement_cost_(movement_cost)
	, radius_(radius)
	, position_(position) {}

std::uint64_t GodotSoilPiece::get_id() const {
	return id_;
}

std::uint8_t GodotSoilPiece::get_type() const {
	return type_;
}

godot::String GodotSoilPiece::get_type_name() const {
	switch (type_) {
		case SAND:
			return "SAND";
		case ROCK:
			return "ROCK";
		case DIRT:
			return "DIRT";
		case WATER:
			return "WATER";
		default:
			return "UNKNOWN";
	}
}

godot::Array GodotSoilPiece::get_properties() const {
	return properties_;
}

godot::Array GodotSoilPiece::get_required_capabilities() const {
	return required_capabilities_;
}

bool GodotSoilPiece::has_damage() const {
	return has_damage_;
}

float GodotSoilPiece::get_damage() const {
	return damage_;
}

bool GodotSoilPiece::has_movement_cost() const {
	return has_movement_cost_;
}

float GodotSoilPiece::get_movement_cost() const {
	return movement_cost_;
}

double GodotSoilPiece::get_radius() const {
	return radius_;
}

godot::Ref<GodotRadius> GodotSoilPiece::get_radius_ref() const {
	return GodotRadius::create(radius_);
}

godot::Vector3 GodotSoilPiece::get_position() const {
	return position_;
}

godot::Ref<GodotSoilPiece> GodotSoilPiece::create(std::uint64_t         id,
												  std::uint8_t          type,
												  const godot::Array   &properties,
												  const godot::Array   &required_capabilities,
												  bool                  has_damage,
												  float                 damage,
												  bool                  has_movement_cost,
												  float                 movement_cost,
												  double                radius,
												  const godot::Vector3 &position) {
	godot::Ref<GodotSoilPiece> ref;
	ref.instantiate();
	ref->id_                    = id;
	ref->type_                  = type;
	ref->properties_            = properties;
	ref->required_capabilities_ = required_capabilities;
	ref->has_damage_            = has_damage;
	ref->damage_                = damage;
	ref->has_movement_cost_     = has_movement_cost;
	ref->movement_cost_         = movement_cost;
	ref->radius_                = radius;
	ref->position_              = position;
	return ref;
}
