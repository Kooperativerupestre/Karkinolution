#pragma once

#include "model/creature/creature.hpp"
#include "model/entity.hpp"

#include <cstdint>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/variant.hpp>
#include <unordered_map>

class GodotEntityStorage : public godot::RefCounted {
		GDCLASS(GodotEntityStorage, godot::RefCounted);

	private:

		std::unordered_map<std::uint64_t, GodotEntity> entities_{};

	protected:

		static void _bind_methods();

	public:

		GodotEntityStorage() = default;

		bool                         add_creature(std::uint64_t id, const godot::Ref<GodotCreature> &creature);
		bool                         remove(std::uint64_t id);
		void                         clear();
		[[nodiscard]] godot::Variant get(std::uint64_t id) const;
		[[nodiscard]] godot::Array   list_ids() const;
		[[nodiscard]] godot::Array   list_entities() const;
		bool                         replace(std::uint64_t old_id, std::uint64_t new_id, const godot::Variant &entity);

		[[nodiscard]] bool           has(std::uint64_t id) const;
		[[nodiscard]] std::int64_t   size() const;
};
