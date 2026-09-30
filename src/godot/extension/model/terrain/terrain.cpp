#include "godot/extension/model/terrain/terrain.hpp"

void GodotTerrain::_bind_methods() {
	godot::ClassDB::bind_method(godot::D_METHOD("get_size"), &GodotTerrain::get_size);
	godot::ClassDB::bind_method(godot::D_METHOD("set_size", "size"), &GodotTerrain::set_size);

	godot::ClassDB::bind_method(godot::D_METHOD("get_soil", "id"), &GodotTerrain::get_soil);
	godot::ClassDB::bind_method(godot::D_METHOD("has_soil", "id"), &GodotTerrain::has_soil);
	godot::ClassDB::bind_method(godot::D_METHOD("list_soils"), &GodotTerrain::list_soils);
	godot::ClassDB::bind_method(godot::D_METHOD("list_ids"), &GodotTerrain::list_ids);

	godot::ClassDB::bind_method(godot::D_METHOD("add_soil", "soil"), &GodotTerrain::add_soil);
	godot::ClassDB::bind_method(godot::D_METHOD("remove_soil", "id"), &GodotTerrain::remove_soil);
	godot::ClassDB::bind_method(godot::D_METHOD("update_soil", "id", "soil"),
								&GodotTerrain::update_soil);

	godot::ClassDB::bind_method(godot::D_METHOD("clear"), &GodotTerrain::clear);
	godot::ClassDB::bind_method(godot::D_METHOD("size"), &GodotTerrain::size);

	godot::ClassDB::bind_static_method("GodotTerrain",
									   godot::D_METHOD("create", "size"),
									   &GodotTerrain::create,
									   DEFVAL(godot::Ref<GodotSize>()));
}

GodotTerrain::GodotTerrain()
	: size_()
	, soils_() {}

GodotTerrain::GodotTerrain(const godot::Ref<GodotSize> &size)
	: size_(size)
	, soils_() {}

godot::Ref<GodotSize> GodotTerrain::get_size() const {
	return size_;
}

void GodotTerrain::set_size(const godot::Ref<GodotSize> &size) {
	size_ = size;
}

godot::Ref<GodotSoilPiece> GodotTerrain::get_soil(std::uint64_t id) const {
	auto it = soils_.find(id);
	if (it == soils_.end()) {
		return godot::Ref<GodotSoilPiece>();
	}
	return it->second;
}

bool GodotTerrain::has_soil(std::uint64_t id) const {
	return soils_.contains(id);
}

godot::Array GodotTerrain::list_soils() const {
	godot::Array result;
	result.resize(static_cast<std::int64_t>(soils_.size()));
	std::int64_t idx = 0;
	for (const auto &[_, soil] : soils_) {
		result[idx++] = soil;
	}
	return result;
}

godot::Array GodotTerrain::list_ids() const {
	godot::Array result;
	result.resize(static_cast<std::int64_t>(soils_.size()));
	std::int64_t idx = 0;
	for (const auto &[id, _] : soils_) {
		result[idx++] = id;
	}
	return result;
}

bool GodotTerrain::add_soil(const godot::Ref<GodotSoilPiece> &soil) {
	if (soil.is_null()) {
		return false;
	}
	const auto id       = soil->get_id();
	auto [it, inserted] = soils_.emplace(id, soil);
	return inserted;
}

bool GodotTerrain::remove_soil(std::uint64_t id) {
	return soils_.erase(id) > 0;
}

bool GodotTerrain::update_soil(std::uint64_t id, const godot::Ref<GodotSoilPiece> &soil) {
	if (soil.is_null()) {
		return false;
	}
	auto it = soils_.find(id);
	if (it == soils_.end()) {
		return false;
	}
	it->second = soil;
	return true;
}

void GodotTerrain::clear() {
	soils_.clear();
}

std::int64_t GodotTerrain::size() const {
	return static_cast<std::int64_t>(soils_.size());
}

godot::Ref<GodotTerrain> GodotTerrain::create(const godot::Ref<GodotSize> &size) {
	godot::Ref<GodotTerrain> ref;
	ref.instantiate();
	ref->size_ = size;
	return ref;
}
