#pragma once

#include "godot/extension/client/alias.hpp"
#include "godot/extension/model/aliases.hpp"
#include "godot_cpp/classes/ref.hpp"
#include "godot_cpp/classes/ref_counted.hpp"

namespace KarkinolutionGodot {
	namespace World {


		namespace Query {

			namespace Creature {


				godot::Ref<GodotCreature> see_creature(godot::Ref<GodotClient> client,
													   std::int64_t            id);
			}; // namespace Creature


		} // namespace Query
	} // namespace World
} // namespace KarkinolutionGodot
