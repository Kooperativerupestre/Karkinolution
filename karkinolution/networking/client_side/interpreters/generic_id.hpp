#pragma once

#include "karkinolution/binary/core_aliases.hpp"

namespace Karkinolution::Networking::ClientSide::Interpreters {
	inline constexpr RequestId GENERIC_INTERPRETER_ID = 45;
} // namespace Karkinolution::Networking::ClientSide::Interpreters

/*
 * This ID is reserved for generic interpreters.
 * Do not use it for actual requests.
 */
