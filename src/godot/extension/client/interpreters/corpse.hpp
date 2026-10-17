#pragma once

#include "godot/extension/client/alias.hpp"
#include "godot/extension/model/aliases.hpp"
#include "godot_cpp/classes/ref.hpp"
#include "godot_cpp/classes/ref_counted.hpp"

#include <godot/extension/client/alias.hpp>

namespace KarkinolutionGodot {
	namespace Interpreters {
		class CorpseInterpreter : public godot::RefCounted {
				GDCLASS(CorpseInterpreter, godot::RefCounted);

			protected:

				static void _bind_methods();

			public:

				static godot::Ref<GodotCorpse> see_corpse(godot::Ref<GodotClient> client,
														  std::int64_t            id);
		};


	} // namespace Interpreters
} // namespace KarkinolutionGodot