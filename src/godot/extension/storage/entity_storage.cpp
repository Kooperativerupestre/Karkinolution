#include "godot/extension/storage/entity_storage.hpp"

void GodotEntityStorage::_bind_methods() {
	godot::ClassDB::bind_method(godot::D_METHOD("add_creature", "id", "creature"),
								&GodotEntityStorage::add_creature);
	godot::ClassDB::bind_method(godot::D_METHOD("delete", "id"), &GodotEntityStorage::remove);
	godot::ClassDB::bind_method(godot::D_METHOD("remove", "id"), &GodotEntityStorage::remove);
	godot::ClassDB::bind_method(godot::D_METHOD("clear"), &GodotEntityStorage::clear);
	godot::ClassDB::bind_method(godot::D_METHOD("get", "id"), &GodotEntityStorage::get);
	godot::ClassDB::bind_method(godot::D_METHOD("list_ids"), &GodotEntityStorage::list_ids);
	godot::ClassDB::bind_method(godot::D_METHOD("list_entities"),
								&GodotEntityStorage::list_entities);
	godot::ClassDB::bind_method(godot::D_METHOD("replace", "old_id", "new_id", "entity"),
								&GodotEntityStorage::replace);
	godot::ClassDB::bind_method(godot::D_METHOD("has", "id"), &GodotEntityStorage::has);
	godot::ClassDB::bind_method(godot::D_METHOD("size"), &GodotEntityStorage::size);
}

bool GodotEntityStorage::add_creature(std::uint64_t id, const godot::Ref<GodotCreature> &creature) {
	if (creature.is_null()) {
		return false;
	}
	auto [it, inserted] = entities_.emplace(id, creature);
	return inserted;
}

bool GodotEntityStorage::remove(std::uint64_t id) {
	return entities_.erase(id) > 0;
}

void GodotEntityStorage::clear() {
	entities_.clear();
}

godot::Variant GodotEntityStorage::get(std::uint64_t id) const {
	auto it = entities_.find(id);
	if (it == entities_.end()) {
		return godot::Variant();
	}

	return std::visit(
		[](const auto &entity) -> godot::Variant {
			return godot::Variant(entity);
		},
		it->second);
}

godot::Array GodotEntityStorage::list_ids() const {
	godot::Array ids;
	ids.resize(static_cast<std::int64_t>(entities_.size()));
	std::int64_t idx = 0;
	for (const auto &[id, _] : entities_) {
		ids[idx++] = id;
	}
	return ids;
}

godot::Array GodotEntityStorage::list_entities() const {
	godot::Array result;
	result.resize(static_cast<std::int64_t>(entities_.size()));
	std::int64_t idx = 0;
	for (const auto &[_, entity] : entities_) {
		std::visit(
			[&](const auto &val) {
				result[idx++] = godot::Variant(val);
			},
			entity);
	}
	return result;
}

bool GodotEntityStorage::replace(std::uint64_t         old_id,
								 std::uint64_t         new_id,
								 const godot::Variant &entity) {
	auto it = entities_.find(old_id);
	if (it == entities_.end()) {
		return false;
	}

	auto converted = GodotEntityConversion::to_entity(entity);
	if (!converted.has_value()) {
		return false;
	}

	if (old_id == new_id) {
		it->second = std::move(*converted);
		return true;
	}

	entities_.erase(it);
	entities_[new_id] = std::move(*converted);
	return true;
}

bool GodotEntityStorage::has(std::uint64_t id) const {
	return entities_.contains(id);
}

std::int64_t GodotEntityStorage::size() const {
	return static_cast<std::int64_t>(entities_.size());
}
