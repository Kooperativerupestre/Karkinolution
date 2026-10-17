#pragma once

#include <cstdint>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/vector3.hpp>

namespace KarkinolutionGodot {
	namespace Models {
		namespace Organisms {
			namespace Creature {
				class GodotCreature : public godot::RefCounted {
						GDCLASS(GodotCreature, godot::RefCounted);

					private:

						std::int64_t   id_{0};
						std::uint8_t   gender_{0};
						std::uint8_t   specie_{0};
						godot::Vector3 position_{0.0, 0.0, 0.0};
						godot::String  name_;


						GodotCreature();

					protected:

						static void _bind_methods();

					public:


						GodotCreature(std::int64_t          id,
									  std::uint8_t          gender,
									  std::uint8_t          specie,
									  const godot::Vector3 &position = godot::Vector3(),
									  const godot::String  &name     = godot::String());

						[[nodiscard]] std::int64_t   get_id() const;
						[[nodiscard]] std::uint8_t   get_gender() const;
						[[nodiscard]] std::uint8_t   get_specie() const;
						[[nodiscard]] godot::Vector3 get_position() const;
						[[nodiscard]] godot::String  get_name() const;

						[[nodiscard]] godot::String get_gender_name() const;
						[[nodiscard]] godot::String get_specie_name() const;
				};
			} // namespace Creature
		} // namespace Organisms
	} // namespace Models
} // namespace KarkinolutionGodot