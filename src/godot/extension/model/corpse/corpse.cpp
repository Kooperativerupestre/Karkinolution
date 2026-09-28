#include "godot/extension/model/corpse/corpse.hpp"

#include <karkinolution/binary/deserialization/interpreters/models.hpp>
#include <karkinolution/organism/entities/corpse/corpse.hpp>

void GodotCorpse::_bind_methods() {
	godot::ClassDB::bind_method(godot::D_METHOD("get_id"), &GodotCorpse::get_id);
	godot::ClassDB::bind_method(godot::D_METHOD("get_raw_meat"), &GodotCorpse::get_raw_meat);
	godot::ClassDB::bind_method(godot::D_METHOD("get_position"), &GodotCorpse::get_position);
	godot::ClassDB::bind_method(godot::D_METHOD("get_size"), &GodotCorpse::get_size);

	godot::ClassDB::bind_static_method(
		"GodotCorpse",
		godot::D_METHOD("create", "id", "raw_meat", "position", "size"),
		&GodotCorpse::create,
		DEFVAL(0),
		DEFVAL(0.0f),
		DEFVAL(godot::Vector3()),
		DEFVAL(godot::Ref<GodotSize>()));
}

GodotCorpse::GodotCorpse()
	: id_(0)
	, raw_meat_(0.0f)
	, position_(0.0, 0.0, 0.0)
	, size_() {}

GodotCorpse::GodotCorpse(std::uint64_t                id,
						 float                        raw_meat,
						 const godot::Vector3        &position,
						 const godot::Ref<GodotSize> &size)
	: id_(id)
	, raw_meat_(raw_meat)
	, position_(position)
	, size_(size) {}

std::uint64_t GodotCorpse::get_id() const {
	return id_;
}

float GodotCorpse::get_raw_meat() const {
	return raw_meat_;
}

godot::Vector3 GodotCorpse::get_position() const {
	return position_;
}

godot::Ref<GodotSize> GodotCorpse::get_size() const {
	return size_;
}

godot::Ref<GodotCorpse> GodotCorpse::create(std::uint64_t                id,
											float                        raw_meat,
											const godot::Vector3        &position,
											const godot::Ref<GodotSize> &size) {
	godot::Ref<GodotCorpse> corpse;
	corpse.instantiate();
	corpse->id_       = id;
	corpse->raw_meat_ = raw_meat;
	corpse->position_ = position;
	corpse->size_     = size;
	return corpse;
}

godot::Ref<GodotCorpse> GodotCorpse::from_deserialized(const DesserializedCorpse &deserialized,
													   std::uint64_t              id) {
	godot::Ref<GodotCorpse> corpse;
	corpse.instantiate();
	corpse->id_       = (id != 0) ? id : deserialized.id;
	corpse->raw_meat_ = deserialized.raw_meat.value;
	corpse->position_ = godot::Vector3(static_cast<godot::real_t>(deserialized.position.x),
									   static_cast<godot::real_t>(deserialized.position.y),
									   static_cast<godot::real_t>(deserialized.position.z));
	corpse->size_     = GodotSize::from_values(deserialized.size.lateral.value,
                                           deserialized.size.height.value,
                                           deserialized.size.depth.value);
	return corpse;
}

godot::Ref<GodotCorpse> GodotCorpse::from_core(const ::Corpse &corpse) {
	godot::Ref<GodotCorpse> result;
	result.instantiate();
	result->id_       = corpse.id;
	result->raw_meat_ = corpse.raw_meat.value;
	result->position_ = godot::Vector3(static_cast<godot::real_t>(corpse.position.x),
									   static_cast<godot::real_t>(corpse.position.y),
									   static_cast<godot::real_t>(corpse.position.z));
	result->size_     = GodotSize::from_values(corpse.size.lateral.value,
                                           corpse.size.height.value,
                                           corpse.size.depth.value);
	return result;
}
