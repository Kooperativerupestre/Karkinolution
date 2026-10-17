#include "godot/extension/client/interpreters/creature.hpp"

#include "godot/extension/client/interpreters/utils.hpp"
#include "godot/extension/model/translator.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "karkinolution/binary/deserialization/interpreters/creature/alias.hpp"
#include "karkinolution/binary/deserialization/interpreters/creature/creature.hpp"
#include "karkinolution/core/id.hpp"
#include "karkinolution/networking/sdk/src/builders/creature/creature.hpp"

namespace KarkinolutionGodot {
	namespace Interpreters {
		namespace Creature {
			godot::Ref<GodotCreature> see_creature(godot::Ref<GodotClient> client,
												   std::int64_t            id) {
				const auto request_data =
					KarkinolutionSDK::Builders::Creature::see_creature(static_cast<BaseIdType>(id));

				const auto frame = Utils::execute_request(client, request_data);
				if (!frame) {
					return godot::Ref<GodotCreature>();
				}

				const auto creature_model = CreatureDSI::get_creature(frame->payload, 0);

				const auto godot_creature =
					KarkinolutionGodot::Translator::translate(id, creature_model);

				godot::Ref<GodotCreature> ref;
				ref.instantiate(godot_creature.get_id(),
								godot_creature.get_gender(),
								godot_creature.get_specie(),
								godot_creature.get_position(),
								godot_creature.get_name());

				return ref;
			}
		} // namespace Creature

		void CreatureInterpreter::_bind_methods() {
			godot::ClassDB::bind_static_method("CreatureInterpreter",
											   godot::D_METHOD("see_creature", "client", "id"),
											   &CreatureInterpreter::see_creature);
		}
	} // namespace Interpreters
} // namespace KarkinolutionGodot
