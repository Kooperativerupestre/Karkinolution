#include "karkinolution/networking/sdk/src/builders/corpse/corpse.hpp"

#include "godot/extension/client/query/aliases.hpp"
#include "godot/extension/model/aliases.hpp"

#include <godot/extension/client/query/corpse.hpp>
#include <godot/extension/client/query/utils.hpp>
#include <godot/extension/model/translator.hpp>
#include <karkinolution/binary/deserialization/interpreters/corpse/alias.hpp>
#include <karkinolution/binary/frames/analyzer.hpp>

godot::Ref<GodotCorpse> CorpseQuery::see_corpse(godot::Ref<GodotClient> client, std::int64_t id) {
	const auto request_data = KarkinolutionSDK::Builders::Corpse::see_corpse(id);
	const auto frame        = Utils::execute_request(client, request_data);

	if (!frame) {
		return godot::Ref<GodotCorpse>();
	}

	if (Frames::Analyzer::is_none(frame.value())) {
		return godot::Ref<GodotCorpse>();
	}

	const auto corpse_model = CorpseDSI::get_corpse(frame->payload, 0);

	const auto godot_corpse = KarkinolutionGodot::Translator::translate(corpse_model);

	return godot_corpse;
}