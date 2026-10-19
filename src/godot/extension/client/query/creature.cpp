#include "godot/extension/client/query/creature.hpp"

#include "godot/extension/client/query/aliases.hpp"
#include "godot/extension/client/query/utils.hpp"
#include "godot/extension/model/translator.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "karkinolution/binary/deserialization/interpreters/creature/alias.hpp"
#include "karkinolution/binary/deserialization/interpreters/creature/creature.hpp"
#include "karkinolution/core/id.hpp"
#include "karkinolution/networking/sdk/src/builders/creature/creature.hpp"

godot::Ref<GodotCreature> CreatureQuery::see_creature(godot::Ref<GodotClient> client,
													  std::int64_t            id) {
	const auto request_data =
		KarkinolutionSDK::Builders::Creature::see_creature(static_cast<BaseIdType>(id));

	const auto frame = Utils::execute_request(client, request_data);
	if (!frame) {
		return godot::Ref<GodotCreature>();
	}

	const auto creature_model = CreatureDSI::get_creature(frame->payload, 0);

	const auto godot_creature = KarkinolutionGodot::Translator::translate(creature_model);


	return godot_creature;
}
