#pragma once

#include <cstdint>
#include <godot/godot-cpp/gen/include/godot_cpp/classes/ref_counted.hpp>
#include <godot/godot-cpp/gen/include/godot_cpp/variant/string.hpp>
#include <godot/godot-cpp/include/godot_cpp/classes/ref.hpp>
#include <godot/godot-cpp/include/godot_cpp/core/class_db.hpp>
#include <godot/godot-cpp/include/godot_cpp/variant/vector3.hpp>
#include <godot_cpp/variant/vector3.hpp>

struct DesserializedCreature;
struct Creature;

class GodotCreature : public godot::RefCounted {
		GDCLASS(GodotCreature, godot::RefCounted);

	private:

		std::uint64_t  id_{0};
		std::uint8_t   gender_{0};
		std::uint8_t   specie_{0};
		godot::Vector3 position_{0.0, 0.0, 0.0};
		godot::String  name_;

	protected:

		static void _bind_methods();

	public:

		GodotCreature();
		GodotCreature(std::uint64_t         id,
					  std::uint8_t          gender,
					  std::uint8_t          specie,
					  const godot::Vector3 &position = godot::Vector3(),
					  const godot::String &name      = godot::String());

		[[nodiscard]] std::uint64_t  get_id() const;
		[[nodiscard]] std::uint8_t   get_gender() const;
		[[nodiscard]] std::uint8_t   get_specie() const;
		[[nodiscard]] godot::Vector3 get_position() const;
		[[nodiscard]] godot::String  get_name() const;

		[[nodiscard]] godot::String get_gender_name() const;
		[[nodiscard]] godot::String get_specie_name() const;

		static godot::Ref<GodotCreature>
		from_deserialized(const DesserializedCreature &deserialized, std::uint64_t id = 0);
		static godot::Ref<GodotCreature> from_core(const ::Creature &creature);
};