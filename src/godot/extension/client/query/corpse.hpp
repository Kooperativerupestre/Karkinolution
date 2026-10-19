#pragma once

#include "godot/extension/client/alias.hpp"
#include "godot/extension/model/aliases.hpp"
#include "godot_cpp/classes/ref.hpp"
#include "godot_cpp/classes/ref_counted.hpp"

#include <godot/extension/client/alias.hpp>

namespace KarkinolutionGodot {
	namespace World {
		namespace Query {
			namespace Corpse {
				godot::Ref<GodotCorpse> see_corpse(godot::Ref<GodotClient> client, std::int64_t id);
			}; // namespace Corpse


		} // namespace Query
	} // namespace World
} // namespace KarkinolutionGodot