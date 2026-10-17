#pragma once

#include "godot/extension/client/alias.hpp"
#include "godot/extension/model/aliases.hpp"
#include "godot_cpp/classes/ref.hpp"
#include "godot_cpp/classes/ref_counted.hpp"

namespace KarkinolutionGodot {
	namespace Interpreters {
		namespace Creature {
			godot::Ref<GodotCreature> see_creature(godot::Ref<GodotClient> client, std::int64_t id);
		} // namespace Creature

		class CreatureInterpreter : public godot::RefCounted {
				GDCLASS(CreatureInterpreter, godot::RefCounted);

			protected:

				static void _bind_methods();

			public:

				static godot::Ref<GodotCreature> see_creature(godot::Ref<GodotClient> client,
															  std::int64_t            id) {
					return Creature::see_creature(client, id);
				}
		};
	} // namespace Interpreters
} // namespace KarkinolutionGodot
