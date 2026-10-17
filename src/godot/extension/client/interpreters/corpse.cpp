#include "karkinolution/networking/sdk/src/builders/corpse/corpse.hpp"

#include "godot/extension/client/interpreters/aliases.hpp"

#include <godot/extension/client/interpreters/corpse.hpp>
#include <godot/extension/client/interpreters/utils.hpp>
#include <godot/extension/model/translator.hpp>
#include <karkinolution/binary/deserialization/interpreters/corpse/alias.hpp>

godot::Ref<GodotCorpse> CorpseINTP::see_corpse(godot::Ref<GodotClient> client, std::int64_t id) {
	const auto request_data = KarkinolutionSDK::Builders::Corpse::see_corpse(id);
	const auto frame        = Utils::execute_request(client, request_data);

	if (!frame) {
		return godot::Ref<GodotCreature>();
	}

	const auto corpse_model = CorpseDSI::get_corpse(frame->payload, 0);

	const auto              godot_corpse = KarkinolutionGodot::Translator::translate(corpse_model);
	godot::Ref<GodotCorpse> ref;
	ref.instantiate(godot_corpse.get_id(),
					godot_corpse.get_raw_meat(),
					godot_corpse.get_position(),
					godot_corpse.get_size());
	return ref;
}