#pragma once

#include "godot/extension/client/alias.hpp"
#include "karkinolution/binary/frames/aliases.hpp"

#include <godot_cpp/classes/ref.hpp>
#include <optional>

namespace KarkinolutionGodot {
	namespace Interpreters {
		namespace Utils {
			// Design Decision:
			// Centralize network execution and error checks in this helper so individual interpreters
			// (Creature, Corpse, Soil, etc.) do not duplicate response verification or signal emitting.
			// Low-level frame validation belongs here; interpreters only focus on domain deserialization.
			std::optional<ResponseFrame> execute_request(
				godot::Ref<GodotClient>                            client,
				const KarkinolutionSDK::Client::ClientRequestData &request_data);
		} // namespace Utils
	} // namespace Interpreters
} // namespace KarkinolutionGodot
